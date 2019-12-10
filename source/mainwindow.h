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

namespace Ui {
class MainWindow;
}





class MainWindow : public QMainWindow
{
  Q_OBJECT

public:


  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  void closeEvent(QCloseEvent *event);

  void setupPlotA(QCustomPlot *customPlot);
  void setupratePlot(QCustomPlot *customPlot);
  void setupratePlot_tab2(QCustomPlot *customPlot);

  void setupsignalslot();
  void setupHistoPlot(QCustomPlot *customPlot);

  

private slots:

  void plotRates(char AoB, int event, double key);
  void plotRates_tab2(int eventA, int eventB, int eventC, double key);
  void changeStartchan(int starchan){this->in_startChan=starchan;}

  void histoplot(const vectorDouble &dat1, const vectorDouble &dat2, const vectorDouble &dat3);

  void LinePlot();

  void BegA1(int val){Plot_Win_BoE[0][0][0]=val;LinePlot();}
  void BegA2(int val){Plot_Win_BoE[0][1][0]=val;LinePlot();}
  void BegA3(int val){Plot_Win_BoE[0][2][0]=val;LinePlot();}
  void EndA1(int val){Plot_Win_BoE[0][0][1]=val;LinePlot();}
  void EndA2(int val){Plot_Win_BoE[0][1][1]=val;LinePlot();}
  void EndA3(int val){Plot_Win_BoE[0][2][1]=val;LinePlot();}

  void BegB1(int val){Plot_Win_BoE[1][0][0]=val;LinePlot();}
  void BegB2(int val){Plot_Win_BoE[1][1][0]=val;LinePlot();}
  void BegB3(int val){Plot_Win_BoE[1][2][0]=val;LinePlot();}
  void EndB1(int val){Plot_Win_BoE[1][0][1]=val;LinePlot();}
  void EndB2(int val){Plot_Win_BoE[1][1][1]=val;LinePlot();}
  void EndB3(int val){Plot_Win_BoE[1][2][1]=val;LinePlot();}

  void BegC1(int val){Plot_Win_BoE[2][0][0]=val;LinePlot();}
  void BegC2(int val){Plot_Win_BoE[2][1][0]=val;LinePlot();}
  void BegC3(int val){Plot_Win_BoE[2][2][0]=val;LinePlot();}
  void EndC1(int val){Plot_Win_BoE[2][0][1]=val;LinePlot();}
  void EndC2(int val){Plot_Win_BoE[2][1][1]=val;LinePlot();}
  void EndC3(int val){Plot_Win_BoE[2][2][1]=val;LinePlot();}

  void Chang_in_binsinplot(int val){this->in_binsinplot=val;}
  void Chang_in_histStart(int val){this->in_histStart=val;}
  void Chang_in_histEnd(int val){this->in_histEnd=val;}

  void Chang_in_adqtime(double val){this->in_adqtime=val;}

  void Chang_plot1_1(int val){this->tab2_plot[0][0]=val;}
  void Chang_plot1_2(int val){this->tab2_plot[0][1]=val;}
  void Chang_plot2_1(int val){this->tab2_plot[1][0]=val;}
  void Chang_plot2_2(int val){this->tab2_plot[1][1]=val;}
  void Chang_plot3_1(int val){this->tab2_plot[2][0]=val;}
  void Chang_plot3_2(int val){this->tab2_plot[2][1]=val;}

  void Chang_win1_1(int val){this->tab2_win[0][0]=val;}
  void Chang_win1_2(int val){this->tab2_win[0][1]=val;}
  void Chang_win2_1(int val){this->tab2_win[1][0]=val;}
  void Chang_win2_2(int val){this->tab2_win[1][1]=val;}
  void Chang_win3_1(int val){this->tab2_win[2][0]=val;}
  void Chang_win3_2(int val){this->tab2_win[2][1]=val;}

  void Chang_in_PlotAChn1(int val){this->in_PlotACh1=val;}
  void Chang_in_PlotAChn2(int val){this->in_PlotACh2=val;}
  void Chang_in_PlotBChn1(int val){this->in_PlotBCh1=val;}
  void Chang_in_PlotBChn2(int val){this->in_PlotBCh2=val;}
  void Chang_in_PlotCChn1(int val){this->in_PlotCCh1=val;}
  void Chang_in_PlotCChn2(int val){this->in_PlotCCh2=val;}

  void Chang_track1(bool val){this->P_T[0]=val;trackRateChang =true;}
  void Chang_track2(bool val){this->P_T[1]=val;trackRateChang =true;}
  void Chang_track3(bool val){this->P_T[2]=val;trackRateChang =true;}
  void Chang_track4(bool val){this->P_T[3]=val;trackRateChang =true;}
  void Chang_track5(bool val){this->P_T[4]=val;trackRateChang =true;}
  void Chang_track6(bool val){this->P_T[5]=val;trackRateChang =true;}
  void Chang_track7(bool val){this->P_T[6]=val;trackRateChang =true;}
  void Chang_track8(bool val){this->P_T[7]=val;trackRateChang =true;}
  void Chang_track9(bool val){this->P_T[8]=val;trackRateChang =true;}

  void CombinationChange(bool val){CombiChang =val;}

  void Chang_adqtime_2(double val){in_adqtime_2=val;}

  void turnONDB(int val);

  void SaveState(bool a);
  void LoadState(bool a);

  void tab2_plot1_activate(bool val){in_tab2_plot1=val;}
  void tab2_plot2_activate(bool val){in_tab2_plot2=val;}
  void tab2_plot3_activate(bool val){in_tab2_plot3=val;}

private:
  Ui::MainWindow *ui;
  qutagadq adq;
  qutaganl anl;
  DBControl dbc;
  QString demoName;
  QTimer dataTimer;
  QCPItemTracer *itemDemoPhaseTracer;
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
  double lastPointKey_tab2;

  ////first tab//////
  QMap<QString, int>windows;

  ///general Configs////
  int in_binsinplot, in_startChan, in_histStart, in_histEnd;
  double in_adqtime;
  int in_PlotACh1, in_PlotACh2, in_PlotBCh1, in_PlotBCh2,in_PlotCCh1,in_PlotCCh2;
  /////first plot////
  int P_R[9];
  bool P_T[9];
  int Plot_Win_BoE[3][3][2];
  //int PA_B1,PA_B2, PA_B3, PA_E1, PA_E2, PA_E3;
  //bool PA_T1, PA_T2, PA_T3;
  //int PA_R1=0, PA_R2=0, PA_R3=0;
  /////second plot////
  //int PB_B1,PB_B2, PB_B3, PB_E1, PB_E2, PB_E3;
  //bool PB_T1, PB_T2, PB_T3;
  //int PB_R1=0, PB_R2=0, PB_R3=0;

  ////SECOND tab////


  float adqtime_tab2;
  int tab2_plot[3][2];
  int tab2_win[3][2];

  bool in_tab2_plot1=false;
  bool in_tab2_plot2=false;
  bool in_tab2_plot3=false;



signals:
    
};

#endif // MAINWINDOW_H
