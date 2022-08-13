#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "qcustomplot.h" 
#include <QtCore>
#include <ctime>
#include <stdio.h>
#include "dbcontrol.h"
#include "qkd_param.h"
#include "save_dialog.h"
#include "ps.h"

#include <QFile>
#include <QDateTime>
#include "swabian.h"
#include <unistd.h> /* For usleep() */
#include "logging.h"
#include "fit.h"
#include <math.h>

#define MAX_QUBITS 1000
#define HDF5TIMEINTEGRATION 3

/* Maximum number of channels on the Swabian. Technically there are 18
 * channels, but I think we only paid for the version with 9 channels. */
#define MAX_CHANNELS 9

typedef QVector<int64_t> vectorInt64;
typedef QVector<int8_t> vectorInt8;
typedef QVector<double> vectorDouble;
typedef QVector<int32_t> vectorInt32;

namespace Ui {
    class MainWindow;
}

extern char qubit_sequence[100000];

/* Struct to hold the quantum bit error rate for the last histogram. We need a
 * struct here since we have several threads involved in calculating this and
 * so we need a vector protected by a mutex lock to pass the data around.
 * First, the HistogramWorkerThread takes the data and passes it off to the
 * main thread. The main thread then calculates the quantum bit error rate and
 * adds it to a qber std::vector. The PhaseStabilizationThread then accesses
 * the vector and matches it up with the voltage that was set in order to
 * figure out what voltage to minimize the error rate.
 *
 * Note: We actually use the success rate instead of the error rate since that
 * should have a smaller relative error. */
typedef struct qber {
    unsigned long long start;
    unsigned long long stop;
    double success;
    double std_success;
} qber;

/* Sturct to hold the quantum bit error rate after it has been matched up with
 * the voltage. */
typedef struct qber_results {
    unsigned long long timestamp;
    double voltage;
    double success;
    double std_success;
} qber_results;

/* Returns the current UNIX time in microseconds. */
unsigned long long microtime(void);

/* Phase Stabilization worker class.
 *
 * This thread is responsible for talking with the LeCroy power supply which
 * supplies a voltage to the heater (?) in Bob's interferometer in order to
 * phase lock it with Alice's interferometer. The voltage is varied and matched
 * up with the quantum bit error (or success) rate and then the voltage is
 * tuned in order to minimize the error rate. This is inherently tricky since
 * we are trying to tune the system to be at a minimum where the first
 * derivative is zero. This means that we can't run a PID algorithm since when
 * the error rate increases we don't know which way to change the voltage.
 *
 * The solution being used right now is that we keep track of the voltage and
 * error rates and fit a sine curve to the data to determine the minimum. In
 * order to make sure we have enough data along the curve we add a sinusoidal
 * dither signal to the voltage on top of where we think the minimum is.
 *
 * This *kind of* seems to work, but does have some stability issues. For one,
 * when you only have points very close to the minimum the fit is not able to
 * determine the amplitude, offset, or frequency of the sine curve. The points
 * are often degenerate with a very fast oscillating wave with a small
 * amplitude and small offset. Therefore, right now I have added priors to the
 * fit to essentially fix these three parameters and only vary the phase (which
 * is what we really care about). In the future it might be nice to collect a
 * lot of data before starting, fit with all parameters free, and then fix
 * these to the values from the first fit. */
class PhaseStabilizationThread : public QThread
{
    Q_OBJECT
public:
    void run() override {
        size_t i;
        char cmd[1024];
        char resp[1024];
        unsigned long timestamp;
        double voltage = 1.0;
        double target = 1.0;
        double min;
        double alpha = 0.01;

        while (!ps_ready(ps)) {
            if (QThread::currentThread()->isInterruptionRequested())
                return;

            usleep(100);
        }

        Log(DEBUG,"socket is ready!");

        ps_query(ps,"*idn?",resp,sizeof(resp));
        Log(DEBUG, "*idn? = '%s'", resp);

        /* Turn Ch. 1 off */
        ps_cmd(ps,":OUTPut1:STATe 0");
        /* Turn on overvoltage protection. */
        ps_cmd(ps,":OUTPut1:OVP:STATe 1");
        ps_cmd(ps,":OUTPut1:OVP 5");
        /* Turn on overcurrent protection. */
        ps_cmd(ps,":OUTPut1:OCP:STATe 1");
        ps_cmd(ps,":OUTPut1:OCP 1");
        /* Set current to 1 A. */
        ps_cmd(ps,":SOURce1:CURRent 1.0");
        /* Set voltage to 1 V. */
        sprintf(cmd,":SOURce1:VOLTage %f", voltage);
        ps_cmd(ps,cmd);
        /* Set channel 1 to constant voltage mode. */
        ps_cmd(ps,":LOAD1:CV OFF");
        ps_cmd(ps,":LOAD1:CC OFF");
        ps_cmd(ps,":LOAD1:CR OFF");
        /* Turn on ch. 1. */
        ps_cmd(ps,":OUTPut1:STATe 1");
        timestamp = microtime();

        int count = 0;
        while (true) {
            if (QThread::currentThread()->isInterruptionRequested())
                return;

            if (!running) {
                usleep(1000);
                continue;
            }

            pthread_mutex_lock(&this->m);
            /* Only add data points taken at least 10 seconds since the
             * voltage was changed. The reason for this is that even though
             * the datasheet on the interferometer says the response time
             * is < 1 second, you can see when changing the voltage that it
             * takes at least ~7 seconds to completely settle.  */
            for (i = 0; i < qber_array.size(); i++) {
                if (qber_array[i].start > timestamp + 10000000) {
                    qber_results_array.push_back(qber_results());
                    qber_results_array.back().timestamp = qber_array[i].start;
                    qber_results_array.back().voltage = voltage;
                    qber_results_array.back().success = qber_array[i].success;
                    qber_results_array.back().std_success = qber_array[i].std_success;
                    Log(VERBOSE, "v = %f err = %f", voltage, qber_results_array.back().success);
                }
            }
            qber_array.clear();
            pthread_mutex_unlock(&this->m);

            /* Only fit the last 100 points. */
            int extra = qber_results_array.size() - 100;
            if (extra > 0) {
                Log(VERBOSE, "deleting %i elements from qber_results_array", extra);
                qber_results_array.erase(qber_results_array.begin(), qber_results_array.begin() + extra);
            }

            /* Only update the voltage once every 20 seconds. */
            if (microtime() < timestamp + 20000000) continue;

            if (qber_results_array.size() >= 10) {
                /* We have ten points. Fit the success rate to a cosine
                 * curve, and calculate the best spot to jump. */
                std::vector<double> x;
                std::vector<double> y;
                std::vector<double> y_err;
                for (i = 0; i < qber_results_array.size(); i++) {
                    x.push_back(qber_results_array[i].voltage);
                    y.push_back(qber_results_array[i].success);
                    y_err.push_back(qber_results_array[i].std_success);
                }
                fit(&x,&y,&y_err,&min);
                Log(VERBOSE, "new min calculated at %f V", min);
                /* min is where we think the minimum is, but we don't
                 * want to move too fast since the interferometer
                 * appears to have some hysteresis, i.e.  if you move
                 * around too fast, you don't get reliable results.
                 * Therefore, we use exponential smoothing to slowly
                 * move towards the target value.
                 *
                 * In addition, we add a dither by adding a component
                 * proportional to the cosine of the current iteration.
                 * This makes sure we don't get stuck at the very
                 * bottom of the minimum where there is no way to
                 * actually fit the cosine curve. */
                target = (1-alpha)*target + alpha*min;
                voltage = target + 0.05*cos(2*M_PI*count++/100.0);
            } else {
                /* We don't have enough points yet. So just move the
                 * voltage up by a fixed amount to map out the curve. */
                voltage += 0.1;
                target = voltage;
            }

            /* Make sure we are not out of range. */
            if (isnan(voltage))
                voltage = 0;

            if (voltage < 0)
                voltage = 0;

            if (voltage > 5)
                voltage = 5;

            Log(VERBOSE, "setting voltage to %f", voltage);

            pthread_mutex_lock(this->sync);
            sprintf(cmd,":SOURce1:VOLTage %f", voltage);
            pthread_mutex_unlock(this->sync);
            ps_cmd(ps,cmd);
            emit(voltage_changed(voltage));
            timestamp = microtime();
        }
    }
    PhaseStabilizationThread(PowerSupply *ps_, pthread_mutex_t *sync_) {
        this->ps = ps_;
        pthread_mutex_init(&this->m,NULL);
        this->sync = sync_;
        this->running = 0;
        this->initialized = 0;
    }
    PowerSupply *ps;
    std::vector<qber> qber_array;
    std::vector<qber_results> qber_results_array;
    pthread_mutex_t m;
    pthread_mutex_t *sync;
    int running;
    int initialized;
signals:
    void voltage_changed(double voltage);
};

/* Histogram worker class. This is a QThread that gets the histogram data from
 * the Swabian class and then emits a signal with the data so that it can be
 * plotted. It's started when the GUI boots up and runs an infinite loop. */
class HistogramWorkerThread : public QThread
{
    Q_OBJECT
public:
    void run() override {
        size_t i;

        while (true) {
            unsigned long long start, stop;
            std::vector<double> dataA, dataB, dataC;

            if (QThread::currentThread()->isInterruptionRequested()) {
                return;
            }

            if (!s->t) {
                sleep(1);
                continue;
            }

            Log(DEBUG, "calling get_histograms()");

            int last_bin_width = this->bin_width;
            pthread_mutex_lock(this->sync);
            start = microtime();
            s->get_histograms(this->start_channel, this->chanA, this->chanB, this->chanC, last_bin_width, this->time, dataA, dataB, dataC);
            stop = microtime();
            pthread_mutex_unlock(this->sync);

            Log(DEBUG, "done calling get_histograms()");

            QVector<double> dataA_q, dataB_q, dataC_q;

            for (i = 0; i < dataA.size(); i++)
                dataA_q.push_back(dataA[i]);
            for (i = 0; i < dataB.size(); i++)
                dataB_q.push_back(dataB[i]);
            for (i = 0; i < dataC.size(); i++)
                dataC_q.push_back(dataC[i]);

            emit(histograms_ready(dataA_q,dataB_q,dataC_q,last_bin_width,start,stop));
        }
    }
    HistogramWorkerThread(Swabian *s_, int start_channel_, int chanA_, int chanB_, int chanC_, int bin_width_, timestamp_t time_, pthread_mutex_t *sync_) {
        this->s = s_;
        this->start_channel = start_channel_;
        this->chanA = chanA_;
        this->chanB = chanB_;
        this->chanC = chanC_;
        this->bin_width = bin_width_;
        this->time = time_;
        this->sync = sync_;
    }
    Swabian *s;
    int start_channel;
    int chanA, chanB, chanC;
    int bin_width;
    timestamp_t time;
    pthread_mutex_t *sync;
signals:
    void histograms_ready(const vectorDouble &datA, const vectorDouble &datB, const vectorDouble &datC, int bin_width, unsigned long long start, unsigned long long stop);
};

/* Count worker class. This is a QThread that gets the event rate on each
 * channel from the Swabian class and then emits a signal with the rates which
 * is picked up by the MainWindow class and displayed on the Parameters tab. */
class CountWorkerThread : public QThread
{
    Q_OBJECT
public:
    void run() override {
        int i, n;
        while (true) {
            int *channels = new int[18];
            double *rates = new double[18];

            if (QThread::currentThread()->isInterruptionRequested()) {
                return;
            }

            if (!s->t) {
                sleep(1);
                continue;
            }

            n = 0;
            for (i = 0; i < 18; i++) {
                if (enabled_mask & (1 << i))
                    channels[n++] = i+1;
            }

            for (i = 0; i < 18; i++)
                rates[i] = 0.0;

            Log(DEBUG, "calling get_count_rates()");

            s->get_count_rates(channels,rates,n);

            Log(DEBUG, "done calling get_count_rates()");

            emit(rates_ready(channels,rates,n));
        }
    }
    CountWorkerThread(Swabian *s_, int enabled_mask_) {
        s = s_;
        enabled_mask = enabled_mask_;
    }
    Swabian *s;
    int enabled_mask;
signals:
    void rates_ready(int *channels, double *rates, int n);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);

    void setupsignalslot();
    void setupHistoPlot(QCustomPlot *customPlot, bool top=true, bool yaxis=true);

    void setup_histolines_Teleport();
  
    void setupDefaultRanges();
  
    void setup_plot_qkd_results(QCustomPlot *scope);
    void setup_plot_qkd_stats(QCustomPlot *scope);
    void setup_plot_voltage(QCustomPlot *scope);

    HistogramWorkerThread *histogramWorkerThread;
    CountWorkerThread *countWorkerThread;
    PhaseStabilizationThread *phaseStabilizationThread;

    int enabled_mask;
    int prev_startChan;
    int prev_chanA;
    int prev_chanB;
    int prev_chanC;

    PowerSupply *ps;
    pthread_mutex_t sync;

private slots:
    void qubit_sequence_changed(void);
    void replot_histograms();
    void refreshAction(QAction *action);
    void disconnectAction(void);
    void connectAction(QAction *action);
    void refreshButton();
    void parametersChanged();
    void histogramChanged(void);
    void show_rates(int *channels, double *rates, int n);
    void show_histograms(const vectorDouble &datA, const vectorDouble &datB, const vectorDouble &datC, int bin_width, unsigned long long start, unsigned long long stop);
    void plot_voltage(double voltage);
    void DrawExpectedSignal(void);
  
    void turnONDB(int val);

    void SaveStateDialog(void);
    void SaveState(QString fileName);
    void LoadStateDialog(void);
    void LoadState(QString fileName, bool warnDialog=true);

    void createQKDLinesA();
    void createQKDLinesB();
    void createQKDLinesC();

    void setup_histolines_QKD();

    void chang_QKD_timeA(double val){in_QKD_timeA=val;if(initR)createQKDLinesA();}
    void chang_QKD_phA(double val){in_QKD_phA=val;if(initR)createQKDLinesA();}
    void chang_QKD_numbA(int val){if(in_QKD_numbA>val)hidelinesA(val);in_QKD_numbA=val;if(initR)createQKDLinesA();}
    void chang_QKD_iwA(int val){in_QKD_iwA=val;if(initR)createQKDLinesA();}
    void chang_QKD_zeroA(int val){in_QKD_zeroA =val;if(initR)createQKDLinesA();}

    void chang_QKD_timeB(double val){in_QKD_timeB=val;if(initR)createQKDLinesB();}
    void chang_QKD_phB(double val){in_QKD_phB=val;if(initR)createQKDLinesB();}
    void chang_QKD_numbB(int val){if(in_QKD_numbB>val)hidelinesB(val);in_QKD_numbB=val;if(initR)createQKDLinesB();}
    void chang_QKD_iwB(int val){in_QKD_iwB=val;if(initR)createQKDLinesB();}
    void chang_QKD_zeroB(int val){in_QKD_zeroB =val;if(initR)createQKDLinesB();}

    void chang_QKD_timeC(double val){in_QKD_timeC=val;if(initR)createQKDLinesC();}
    void chang_QKD_phC(double val){in_QKD_phC=val;if(initR)createQKDLinesC();}
    void chang_QKD_numbC(int val){if(in_QKD_numbC>val)hidelinesC(val);in_QKD_numbC=val;if(initR)createQKDLinesC();}
    void chang_QKD_iwC(int val){in_QKD_iwC=val;if(initR)createQKDLinesC();}
    void chang_QKD_zeroC(int val){in_QKD_zeroC =val;if(initR)createQKDLinesC();}

    void hidelinesA(int val);
    void hidelinesB(int val);
    void hidelinesC(int val);

    void set_qkd_datafromDB(const boolvector2d &dat,int qkdcolumns, int qkdrows);

    void plot_qkd_results_det(double okA,double errA,double randA,double bkgndA,double okB,double errB,double randB,double bkgndB,double okC,double errC,double randC,double bkgndC, double key);
    void plot_qkd_results_QB(double okE,double errE,double randE,double bkgndE,double okL,double errL,double randL, double bkgndL,double okP,double errP,double randP,double bkgndP, double key);

    void plot_qkd_stats(double sifted_time, double sifted_phase, double error_time, double error_phase, double error_phase_qubit, double key);

    void fillTablesNames(QStringList tables_names);
    void tableSelected(QAction* action){emit tableQKDtoDB(action->text());}

    void chang_QKD_turnONDB(int val){QKD_DB_ON=val;}

    void PowerSupplyStart(void);
    void PowerSupplyStop(void);
    void PowerSupplyDisconnect(void);
    void PowerSupplyConnect(void);

    void resetButton_clicked();

    void savePageHists(bool h_time, bool h_phase_ok, bool h_phase_bad);
    void savePageDet(bool h1, bool h2, bool h3);
    void savePageQB(bool early, bool late, bool phase);
    void savePageStats(bool time, bool error, bool voltage);
    void saveData(QString file_path, bool h_time, bool h_phase_ok, bool h_phase_bad, bool h1, bool h2, bool h3, bool early, bool late, bool phase, bool time, bool error, bool voltage);

    void pointsButton_clicked(int amount = 3);

private:
    Ui::MainWindow *ui;
    Swabian s;
    DBControl dbc;
    QKD_param qkdparam;
    Save_dialog save_dialog;
    QString demoName;
    QTimer dataTimer;
    QCPItemTracer *itemDemoPhaseTracer;
    bool Teleport0_or_QKD1=1;
    bool dbrunning=false;

    QCPItemStraightLine *infLine[18];

    double lastPointKey_tab1;
    double lastPointKey_tab3;
    double lastPointKey_tab4;
    double qkd_prevKey;

    bool initR=false;
    ////first tab//////
    QMap<QString, int>windows;
    double prevbinwidth;

    ///general Configs////
    int in_histStart, in_histEnd;
    double in_adqtime;
    int in_PlotACh1, in_PlotACh2, in_PlotBCh1, in_PlotBCh2,in_PlotCCh1,in_PlotCCh2;
    /////first plot////
    int P_R[9]={0};
    bool P_T[9]={0};
    int Plot_Win_BoE[3][3][2]={{{0}}};

    ////SECOND tab////


    //////QKD///////////////

    QCPItemStraightLine *LinesPlotA1[MAX_QUBITS];
    QCPItemStraightLine *LinesPlotA2[MAX_QUBITS];
    QCPItemStraightLine *LinesPlotA3[MAX_QUBITS];
    QCPItemStraightLine *LinesPlotA4[MAX_QUBITS];

    QCPItemStraightLine *LinesPlotB1[MAX_QUBITS];
    QCPItemStraightLine *LinesPlotB2[MAX_QUBITS];
    QCPItemStraightLine *LinesPlotB3[MAX_QUBITS];
    QCPItemStraightLine *LinesPlotB4[MAX_QUBITS];

    QCPItemStraightLine *LinesPlotC1[MAX_QUBITS];
    QCPItemStraightLine *LinesPlotC2[MAX_QUBITS];
    QCPItemStraightLine *LinesPlotC3[MAX_QUBITS];
    QCPItemStraightLine *LinesPlotC4[MAX_QUBITS];

    int QubitTime, Phasetime, NoQubits, PeaksQubit, QKD_intWind, QKD_zero;

    int  in_QKD_numbA=30, in_QKD_iwA=45, in_QKD_zeroA=1;
    int  in_QKD_numbB=30, in_QKD_iwB=45, in_QKD_zeroB=1;
    int  in_QKD_numbC=30, in_QKD_iwC=45, in_QKD_zeroC=1;
    double in_QKD_timeA=200, in_QKD_phA=50;
    double in_QKD_timeB=200, in_QKD_phB=50;
    double in_QKD_timeC=200, in_QKD_phC=50;

    QVector<QVector<bool>> in_qkdfromDB;
    int in_qubnumindb, in_qkddbcolumns;
    bool qubitsfromDBloaded = false;
    int QKD_DB_ON=0;

    QVector<int> datah5okA;
    QVector<int> datah5errA;
    QVector<int> datah5randA;
    QVector<int> datah5bkgndA;
    QVector<int> datah5okB;
    QVector<int> datah5errB;
    QVector<int> datah5randB;
    QVector<int> datah5bkgndB;
    QVector<int> datah5okC;
    QVector<int> datah5errC;
    QVector<int> datah5randC;
    QVector<int> datah5bkgndC;

    int rof[18];
    double thresholds[18];
    int delay[18];
    int test[18];

    QComboBox *rof_widgets[18];
    QDoubleSpinBox *threshold_widgets[18];
    QSpinBox *delay_widgets[18];
    QComboBox *test_widgets[18];
    QLCDNumber *rate_widgets[18];

    bool HDF5File_created=false;

    // These 12's must change together if more plots are added for saving.
    const int num_ui_plots = 12;
    QCustomPlot *ui_plots[12];

signals:
    void main_SaveAndValues(int and1, int and2, int and3, int orgate, int bsm1, int bsm2, float andTime, int delayline);
    void main_SaveRateValues( int Ra1, int Ra2, int Ra3, int Rb1, int Rb2, int Rb3, int Rc1, int Rc2, int Rc3, float hist_adqtime);

    void main_SaveQKDresults(double okA, double errA,double  randA, double bkgndA, double okB, double errB, double randB, double bkgndB, double okC, double errC, double randC, double bkgndC);
    void main_SaveQKDstats(int sifted_time, int sifted_phase, double error_time, double error_phase);
    void saveH5datafromMW(const intvector datah5okA, const intvector datah5errA, const intvector datah5randA, const intvector datah5bkgndA, const intvector datah5okB, const intvector datah5errB, const intvector datah5randB, const intvector datah5bkgndB, const intvector datah5okC, const intvector datah5errC, const intvector datah5randC, const intvector datah5bkgndC);
    void tableQKDtoDB(QString text);

    void sig_CreateHDF5(QString file_path);
    void sig_attrHDF5(QKD_param *param, char qubit_sequence[100000], float adq_time);
    void sig_closeHDF5();
};

#endif // MAINWINDOW_H
