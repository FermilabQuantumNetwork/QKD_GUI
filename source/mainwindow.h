#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "qcustomplot.h" 
#include <QtCore>
#include <ctime>
#include <stdio.h>
#include "qutag_adq.h"
#include "qutag_anl.h"
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

#define MAX_QUBITS 1000
#define HDF5TIMEINTEGRATION 3

/* Maximum number of channels on the Swabian. Technically there are 18
 * channels, but I think we only paid for the version with 9 channels. */
#define MAX_CHANNELS 9

namespace Ui {
    class MainWindow;
}

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

            if (!s->t) {
                sleep(1);
                continue;
            }

            s->get_histograms(this->start_channel, this->chanA, this->chanB, this->chanC, this->bin_width, this->time, dataA, dataB, dataC);

            QVector<double> dataA_q, dataB_q, dataC_q;

            for (i = 0; i < dataA.size(); i++)
                dataA_q.push_back(dataA[i]);
            for (i = 0; i < dataB.size(); i++)
                dataB_q.push_back(dataB[i]);
            for (i = 0; i < dataC.size(); i++)
                dataC_q.push_back(dataC[i]);

            emit(histograms_ready(dataA_q,dataB_q,dataC_q));
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
    void histograms_ready(const vectorDouble &datA, const vectorDouble &datB, const vectorDouble &datC);
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
            if (!s->t) {
                sleep(1);
                continue;
            }
            double *rates = new double[18];

            for (i = 0; i < 18; i++)
                rates[i] = 0.0;

            s->get_count_rates(channels,rates,9);

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
    void setupHistoPlot(QCustomPlot *customPlot);

    void setup_histolines_Teleport();
  
    void setupDefaultRanges();
  
    void setup_plot_qkd_results(QCustomPlot *scope);
    void setup_plot_qkd_stats(QCustomPlot *scope);

    HistogramWorkerThread *histogramWorkerThread;
    CountWorkerThread *countWorkerThread;

private slots:
    void refreshButton();
    void connectButton();
    void parametersChanged();
    void histogramChanged(void);
    void show_rates(double *rates);
    void show_histograms(const vectorDouble &datA, const vectorDouble &datB, const vectorDouble &datC);
  
  void plotRates(char AoB, int event, double key);

  void changeStartchan(int starchan){this->in_startChan=starchan;}

  void histoplot(const vectorDouble &dat1, const vectorDouble &dat2, const vectorDouble &dat3);

  void LinePlot();

  void CombinationChange(bool val){CombiChang =val;}

  void turnONDB(int val);

  void SaveState(bool a);
  void LoadState(bool a);

  void resetdelay(){in_delayline=0;prev_homscan=0;}
  void chang_in_max_del(int val){in_Max_delay=val;}

  void createQKDLinesA();
  void createQKDLinesB();
  void createQKDLinesC();


  void setup_histolines_QKD();

  void chang_QKD_timeA(double val){in_QKD_timeA=val;if(initR)createQKDLinesA();}
  void chang_QKD_phA(int val){in_QKD_phA=val;if(initR)createQKDLinesA();}
  void chang_QKD_numbA(int val){if(in_QKD_numbA>val)hidelinesA(val);in_QKD_numbA=val;if(initR)createQKDLinesA();}
  void chang_QKD_pxqA(int val){in_QKD_pxqA=val;if(initR)createQKDLinesA();}
  void chang_QKD_iwA(int val){in_QKD_iwA=val;if(initR)createQKDLinesA();}
  void chang_QKD_zeroA(int val){in_QKD_zeroA =val;if(initR)createQKDLinesA();}

  void chang_QKD_timeB(double val){in_QKD_timeB=val;if(initR)createQKDLinesB();}
  void chang_QKD_phB(int val){in_QKD_phB=val;if(initR)createQKDLinesB();}
  void chang_QKD_numbB(int val){if(in_QKD_numbB>val)hidelinesB(val);in_QKD_numbB=val;if(initR)createQKDLinesB();}
  void chang_QKD_pxqB(int val){in_QKD_pxqB=val;if(initR)createQKDLinesB();}
  void chang_QKD_iwB(int val){in_QKD_iwB=val;if(initR)createQKDLinesB();}
  void chang_QKD_zeroB(int val){in_QKD_zeroB =val;if(initR)createQKDLinesB();}

  void chang_QKD_timeC(double val){in_QKD_timeC=val;if(initR)createQKDLinesC();}
  void chang_QKD_phC(int val){in_QKD_phC=val;if(initR)createQKDLinesC();}
  void chang_QKD_numbC(int val){if(in_QKD_numbC>val)hidelinesC(val);in_QKD_numbC=val;if(initR)createQKDLinesC();}
  void chang_QKD_pxqC(int val){in_QKD_pxqC=val;if(initR)createQKDLinesC();}
  void chang_QKD_iwC(int val){in_QKD_iwC=val;if(initR)createQKDLinesC();}
  void chang_QKD_zeroC(int val){in_QKD_zeroC =val;if(initR)createQKDLinesC();}

  /*int in_QKD_timeA, in_QKD_numbA, in_QKD_phA, in_QKD_iwA, in_QKD_pxqA, in_QKD_zeroA;
  int in_QKD_timeB, in_QKD_numbB, in_QKD_phB, in_QKD_iwB, in_QKD_pxqB, in_QKD_zeroB;
  int in_QKD_timeC, in_QKD_numbC, in_QKD_phC, in_QKD_iwC, in_QKD_pxqC, in_QKD_zeroC;*/

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
  qutagadq adq;
  Swabian s;
  qutaganl anl;
  DBControl dbc;
  socket_com udpcom;
  QKD_param qkdparam;
  QString demoName;
  QTimer dataTimer;
  QCPItemTracer *itemDemoPhaseTracer;
  bool Teleport0_or_QKD1=1;
  int currentDemoIndex;
  double prom;
  QButtonGroup *buttonGroup1 ;
  QButtonGroup *buttonGroup2 ;
  bool trackRateChang =false, CombiChang =false;
  float in_adqtime_2;
  QVector<int> datach1;
  QVector<int> datacali;
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
  int in_startChan;
  double in_adqtime;
  int in_PlotACh1, in_PlotBCh1, in_PlotCCh1;
  /////first plot////
  int P_R[9]={0};
  bool P_T[9]={0};
  int Plot_Win_BoE[3][3][2]={{{0}}};

  ////SECOND tab////


  int in_delayline=0;

  bool in_homscan=false;
  int in_homscan_time;
  int prev_homscan=0;
  int xrange = 120;
  int in_Max_delay=500;
  bool firstscan=false;
  int in_stepduration;
  double del_key, del_previouskey;

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

  int  in_QKD_numbA=30, in_QKD_phA=50, in_QKD_iwA=45, in_QKD_pxqA=3, in_QKD_zeroA=1;
  int  in_QKD_numbB=30, in_QKD_phB=50, in_QKD_iwB=45, in_QKD_pxqB=3, in_QKD_zeroB=1;
  int  in_QKD_numbC=30, in_QKD_phC=50, in_QKD_iwC=45, in_QKD_pxqC=3, in_QKD_zeroC=1;
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
