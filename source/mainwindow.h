#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "qcustomplot.h" 
#include <QtCore>
#include <ctime>
#include <stdio.h>
#include "dbcontrol.h"
#include "socket_com.h"
#include "qkd_param.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include "swabian.h"
#include <unistd.h> /* For usleep() */

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

const bool debug = false;

extern char qubit_sequence[100];

/* Histogram worker class. This is a QThread that gets the histogram data from
 * the Swabian class and then emits a signal with the data so that it can be
 * plotted. It's started when the GUI boots up and runs an infinite loop. */
class HistogramWorkerThread : public QThread
{
    Q_OBJECT
public:
    void run() override {
        int i;

        while (true) {
            std::vector<double> dataA, dataB, dataC;

            if (QThread::currentThread()->isInterruptionRequested()) {
                return;
            }

            if (!s->t) {
                sleep(1);
                continue;
            }

            if (debug)
                fprintf(stderr, "calling get_histograms()\n");

            int last_bin_width = this->bin_width;
            s->get_histograms(this->start_channel, this->chanA, this->chanB, this->chanC, last_bin_width, this->time, dataA, dataB, dataC);

            if (debug)
                fprintf(stderr, "done calling get_histograms()\n");

            QVector<double> dataA_q, dataB_q, dataC_q;

            for (i = 0; i < (int) dataA.size(); i++)
                dataA_q.push_back(dataA[i]);
            for (i = 0; i < (int) dataB.size(); i++)
                dataB_q.push_back(dataB[i]);
            for (i = 0; i < (int) dataC.size(); i++)
                dataC_q.push_back(dataC[i]);

            emit(histograms_ready(dataA_q,dataB_q,dataC_q,last_bin_width));
        }
    }
    HistogramWorkerThread(Swabian *s_, int start_channel_, int chanA_, int chanB_, int chanC_, int bin_width_, timestamp_t time_) {
        this->s = s_;
        this->start_channel = start_channel_;
        this->chanA = chanA_;
        this->chanB = chanB_;
        this->chanC = chanC_;
        bin_width = bin_width_;
        time = time_;
    }
    Swabian *s;
    int start_channel;
    int chanA, chanB, chanC;
    int bin_width;
    timestamp_t time;
signals:
    void histograms_ready(const vectorDouble &datA, const vectorDouble &datB, const vectorDouble &datC, int bin_width);
};

/* Count worker class. This is a QThread that gets the event rate on each
 * channel from the Swabian class and then emits a signal with the rates which
 * is picked up by the MainWindow class and displayed on the Parameters tab. */
class CountWorkerThread : public QThread
{
    Q_OBJECT
public:
    void run() override {
        int i;
        int channels[18];

        for (i = 0; i < 18; i++)
            channels[i] = i+1;
        
        while (true) {
            if (QThread::currentThread()->isInterruptionRequested()) {
                return;
            }

            if (!s->t) {
                sleep(1);
                continue;
            }

            double *rates = new double[18];

            for (i = 0; i < 18; i++)
                rates[i] = 0.0;

            if (debug)
                fprintf(stderr, "calling get_count_rates()\n");

            s->get_count_rates(channels,rates,9);

            if (debug)
                fprintf(stderr, "done calling get_count_rates()\n");

            emit(rates_ready(rates));
        }
    }
    CountWorkerThread(Swabian *s_) {
        s = s_;
    }
    Swabian *s;
signals:
    void rates_ready(double *rates);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);

    void setupPlotA(QCustomPlot *customPlot);
    void setupratePlot(QCustomPlot *customPlot);

    void setupsignalslot();
    void setupHistoPlot(QCustomPlot *customPlot, bool top=true, bool yaxis=true);

    void setup_histolines_Teleport();
  
    void setupDefaultRanges();
  
    void setup_plot_qkd_results(QCustomPlot *scope);
    void setup_plot_qkd_stats(QCustomPlot *scope);

    HistogramWorkerThread *histogramWorkerThread;
    CountWorkerThread *countWorkerThread;

private slots:
    void blah();
    void connectAction(QAction *action);
    void refreshButton();
    void parametersChanged();
    void histogramChanged(void);
    void show_rates(double *rates);
    void show_histograms(const vectorDouble &datA, const vectorDouble &datB, const vectorDouble &datC, int bin_width);
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
    void chang_QKD_phA(int val){in_QKD_phA=val;if(initR)createQKDLinesA();}
    void chang_QKD_numbA(int val){if(in_QKD_numbA>val)hidelinesA(val);in_QKD_numbA=val;if(initR)createQKDLinesA();}
    void chang_QKD_iwA(int val){in_QKD_iwA=val;if(initR)createQKDLinesA();}
    void chang_QKD_zeroA(int val){in_QKD_zeroA =val;if(initR)createQKDLinesA();}

    void chang_QKD_timeB(double val){in_QKD_timeB=val;if(initR)createQKDLinesB();}
    void chang_QKD_phB(int val){in_QKD_phB=val;if(initR)createQKDLinesB();}
    void chang_QKD_numbB(int val){if(in_QKD_numbB>val)hidelinesB(val);in_QKD_numbB=val;if(initR)createQKDLinesB();}
    void chang_QKD_iwB(int val){in_QKD_iwB=val;if(initR)createQKDLinesB();}
    void chang_QKD_zeroB(int val){in_QKD_zeroB =val;if(initR)createQKDLinesB();}

    void chang_QKD_timeC(double val){in_QKD_timeC=val;if(initR)createQKDLinesC();}
    void chang_QKD_phC(int val){in_QKD_phC=val;if(initR)createQKDLinesC();}
    void chang_QKD_numbC(int val){if(in_QKD_numbC>val)hidelinesC(val);in_QKD_numbC=val;if(initR)createQKDLinesC();}
    void chang_QKD_iwC(int val){in_QKD_iwC=val;if(initR)createQKDLinesC();}
    void chang_QKD_zeroC(int val){in_QKD_zeroC =val;if(initR)createQKDLinesC();}

    void hidelinesA(int val);
    void hidelinesB(int val);
    void hidelinesC(int val);

    void set_qkd_datafromDB(const boolvector2d &dat,int qkdcolumns, int qkdrows);

    void plot_qkd_results_det(double okA,double errA,double randA,double bkgndA,double okB,double errB,double randB,double bkgndB,double okC,double errC,double randC,double bkgndC, double key);
    void plot_qkd_results_QB(double okE,double errE,double randE,double bkgndE,double okL,double errL,double randL, double bkgndL,double okP,double errP,double randP,double bkgndP, double key);

    void plot_qkd_stats(double sifted_time, double sifted_phase, double error_time, double error_phase, double key);

    void hdf5savefile();

    void fillTablesNames(QStringList tables_names);
    void tableSelected(QAction* action){emit tableQKDtoDB(action->text());}

    void chang_QKD_turnONDB(int val){QKD_DB_ON=val;}

private:
    Ui::MainWindow *ui;
    Swabian s;
    DBControl dbc;
    socket_com udpcom;
    QKD_param qkdparam;
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

    int  in_QKD_numbA=30, in_QKD_phA=50, in_QKD_iwA=45, in_QKD_zeroA=1;
    int  in_QKD_numbB=30, in_QKD_phB=50, in_QKD_iwB=45, in_QKD_zeroB=1;
    int  in_QKD_numbC=30, in_QKD_phC=50, in_QKD_iwC=45, in_QKD_zeroC=1;
    double in_QKD_timeA=200, in_QKD_timeB=200, in_QKD_timeC=200;

    QVector<QVector<bool>> in_qkdfromDB;
    int in_qubnumindb, in_qkddbcolumns;
    bool qubitsfromDBloaded = false;
    int QKD_DB_ON=0;

    QVector<double> datah5okA;
    QVector<double> datah5errA;
    QVector<double> datah5randA;
    QVector<double> datah5bkgndA;
    QVector<double> datah5okB;
    QVector<double> datah5errB;
    QVector<double> datah5randB;
    QVector<double> datah5bkgndB;
    QVector<double> datah5okC;
    QVector<double> datah5errC;
    QVector<double> datah5randC;
    QVector<double> datah5bkgndC;

    /*QFile savejasonFile;
    QJsonArray jasonhistoA, jasonhistoB, jasonhistoC;*/

    bool HDF5File_created=false;
signals:
    void main_SaveAndValues(int and1, int and2, int and3, int orgate, int bsm1, int bsm2, float andTime, int delayline);
    void main_SaveRateValues( int Ra1, int Ra2, int Ra3, int Rb1, int Rb2, int Rb3, int Rc1, int Rc2, int Rc3, float hist_adqtime);

    void main_SaveQKDresults(double okA, double errA,double  randA, double bkgndA, double okB, double errB, double randB, double bkgndB, double okC, double errC, double randC, double bkgndC);
    void main_SaveQKDstats(int sifted_time, int sifted_phase, double error_time, double error_phase);
    void MW_savehdf5(QString mensaje);
    void saveH5datafromMW(const intvector datah5okA, const intvector datah5errA, const intvector datah5randA, const intvector datah5bkgndA, const intvector datah5okB, const intvector datah5errB, const intvector datah5randB, const intvector datah5bkgndB, const intvector datah5okC, const intvector datah5errC, const intvector datah5randC, const intvector datah5bkgndC);
    void tableQKDtoDB(QString text);
};

#endif // MAINWINDOW_H
