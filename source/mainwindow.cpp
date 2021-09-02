#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>
#include <iostream>//entradas y salidas por consola
#include <fstream>//archivos.txt
#include <vector>//min_element//max_exelement//HD
#include <QApplication>
#include <QtCore>
#include <algorithm>
#include <H5Cpp.h>
#include "qkd_param.h"
#include "ui_qkd_param.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

char qubit_sequence[100] = "E0E0L0L0P0P0";

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setGeometry(200, 200, 1500, 800);
    setupsignalslot();
    setWindowTitle(QString("INQNET TDC"));
    Teleport0_or_QKD1=1;

    // setup style of the histograms and plots
    setupHistoPlot(ui->PlotB);
    setupHistoPlot(ui->PlotB_2,false,false);
    setupHistoPlot(ui->PlotA);
    setupHistoPlot(ui->PlotA_2,false,false);
    setupHistoPlot(ui->PlotC);
    setupHistoPlot(ui->PlotC_2,false,false);
    connect(ui->PlotA->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->PlotA_2->xAxis, SLOT(setRange(QCPRange)));
    connect(ui->PlotA->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(blah()));
    connect(ui->PlotA_2->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->PlotA->xAxis, SLOT(setRange(QCPRange)));
    connect(ui->PlotA_2->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(blah()));

    connect(ui->PlotB->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->PlotB_2->xAxis, SLOT(setRange(QCPRange)));
    connect(ui->PlotB->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(blah()));
    connect(ui->PlotB_2->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->PlotB->xAxis, SLOT(setRange(QCPRange)));
    connect(ui->PlotB_2->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(blah()));

    connect(ui->PlotC->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->PlotC_2->xAxis, SLOT(setRange(QCPRange)));
    connect(ui->PlotC->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(blah()));
    connect(ui->PlotC_2->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->PlotC->xAxis, SLOT(setRange(QCPRange)));
    connect(ui->PlotC_2->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(blah()));

    setup_plot_qkd_results(ui->QKD_H1_results);
    setup_plot_qkd_results(ui->QKD_H2_results);
    setup_plot_qkd_results(ui->QKD_H3_results);
    setup_plot_qkd_stats(ui->qkd_errorplot);
    setup_plot_qkd_stats(ui->qkd_siftedplot);
    setup_plot_qkd_results(ui->Early_results);
    setup_plot_qkd_results(ui->Late_results);
    setup_plot_qkd_results(ui->Phase_results);

    setup_histolines_QKD();

    ui->threshold1->setValue(0);
    ui->threshold2->setValue(0);
    ui->threshold3->setValue(0);
    ui->threshold4->setValue(0);
    ui->threshold5->setValue(0);
    ui->threshold6->setValue(0);
    ui->threshold7->setValue(0);
    ui->threshold8->setValue(0);
    ui->threshold9->setValue(0);
    ui->threshold10->setValue(0);
    ui->threshold11->setValue(0);
    ui->threshold12->setValue(0);
    ui->threshold13->setValue(0);
    ui->threshold14->setValue(0);
    ui->threshold15->setValue(0);
    ui->threshold16->setValue(0);
    ui->threshold17->setValue(0);
    ui->threshold18->setValue(0);

    ui->delay1->setValue(0);
    ui->delay2->setValue(0);
    ui->delay3->setValue(0);
    ui->delay4->setValue(0);
    ui->delay5->setValue(0);
    ui->delay6->setValue(0);
    ui->delay7->setValue(0);
    ui->delay8->setValue(0);
    ui->delay9->setValue(0);
    ui->delay10->setValue(0);
    ui->delay11->setValue(0);
    ui->delay12->setValue(0);
    ui->delay13->setValue(0);
    ui->delay14->setValue(0);
    ui->delay15->setValue(0);
    ui->delay16->setValue(0);
    ui->delay17->setValue(0);
    ui->delay18->setValue(0);

    ui->bin_width->setValue(10);
    // update rate Adq time
    ui->adqtime->setValue(2);

    ui->PlotAChn1->setValue(1);
    ui->PlotBChn1->setValue(1);
    ui->PlotCChn1->setValue(1);
    ui->startChan->setValue(1);

    lastPointKey_tab1 = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    lastPointKey_tab3 = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    lastPointKey_tab4 = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    qkd_prevKey = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;

    ui->rof1->addItem(tr("Rise"));
    ui->rof1->addItem(tr("Fall"));
    ui->rof2->addItem(tr("Rise"));
    ui->rof2->addItem(tr("Fall"));
    ui->rof3->addItem(tr("Rise"));
    ui->rof3->addItem(tr("Fall"));
    ui->rof4->addItem(tr("Rise"));
    ui->rof4->addItem(tr("Fall"));
    ui->rof5->addItem(tr("Rise"));
    ui->rof5->addItem(tr("Fall"));
    ui->rof6->addItem(tr("Rise"));
    ui->rof6->addItem(tr("Fall"));
    ui->rof7->addItem(tr("Rise"));
    ui->rof7->addItem(tr("Fall"));
    ui->rof8->addItem(tr("Rise"));
    ui->rof8->addItem(tr("Fall"));
    ui->rof9->addItem(tr("Rise"));
    ui->rof9->addItem(tr("Fall"));
    ui->rof10->addItem(tr("Rise"));
    ui->rof10->addItem(tr("Fall"));
    ui->rof11->addItem(tr("Rise"));
    ui->rof11->addItem(tr("Fall"));
    ui->rof12->addItem(tr("Rise"));
    ui->rof12->addItem(tr("Fall"));
    ui->rof13->addItem(tr("Rise"));
    ui->rof13->addItem(tr("Fall"));
    ui->rof14->addItem(tr("Rise"));
    ui->rof14->addItem(tr("Fall"));
    ui->rof15->addItem(tr("Rise"));
    ui->rof15->addItem(tr("Fall"));
    ui->rof16->addItem(tr("Rise"));
    ui->rof16->addItem(tr("Fall"));
    ui->rof17->addItem(tr("Rise"));
    ui->rof17->addItem(tr("Fall"));
    ui->rof18->addItem(tr("Rise"));
    ui->rof18->addItem(tr("Fall"));

    ui->rof1->setCurrentText("Fall");
    ui->rof2->setCurrentText("Fall");
    ui->rof3->setCurrentText("Fall");
    ui->rof4->setCurrentText("Fall");
    ui->rof5->setCurrentText("Fall");
    ui->rof6->setCurrentText("Fall");
    ui->rof7->setCurrentText("Fall");
    ui->rof8->setCurrentText("Fall");
    ui->rof9->setCurrentText("Fall");
    ui->rof10->setCurrentText("Fall");
    ui->rof11->setCurrentText("Fall");
    ui->rof12->setCurrentText("Fall");
    ui->rof13->setCurrentText("Fall");
    ui->rof14->setCurrentText("Fall");
    ui->rof15->setCurrentText("Fall");
    ui->rof16->setCurrentText("Fall");
    ui->rof17->setCurrentText("Fall");
    ui->rof18->setCurrentText("Fall");

    ui->test1->addItem(tr("Enable"));
    ui->test1->addItem(tr("Disable"));
    ui->test2->addItem(tr("Enable"));
    ui->test2->addItem(tr("Disable"));
    ui->test3->addItem(tr("Enable"));
    ui->test3->addItem(tr("Disable"));
    ui->test4->addItem(tr("Enable"));
    ui->test4->addItem(tr("Disable"));
    ui->test5->addItem(tr("Enable"));
    ui->test5->addItem(tr("Disable"));
    ui->test6->addItem(tr("Enable"));
    ui->test6->addItem(tr("Disable"));
    ui->test7->addItem(tr("Enable"));
    ui->test7->addItem(tr("Disable"));
    ui->test8->addItem(tr("Enable"));
    ui->test8->addItem(tr("Disable"));
    ui->test9->addItem(tr("Enable"));
    ui->test9->addItem(tr("Disable"));
    ui->test10->addItem(tr("Enable"));
    ui->test10->addItem(tr("Disable"));
    ui->test11->addItem(tr("Enable"));
    ui->test11->addItem(tr("Disable"));
    ui->test12->addItem(tr("Enable"));
    ui->test12->addItem(tr("Disable"));
    ui->test13->addItem(tr("Enable"));
    ui->test13->addItem(tr("Disable"));
    ui->test14->addItem(tr("Enable"));
    ui->test14->addItem(tr("Disable"));
    ui->test15->addItem(tr("Enable"));
    ui->test15->addItem(tr("Disable"));
    ui->test16->addItem(tr("Enable"));
    ui->test16->addItem(tr("Disable"));
    ui->test17->addItem(tr("Enable"));
    ui->test17->addItem(tr("Disable"));
    ui->test18->addItem(tr("Enable"));
    ui->test18->addItem(tr("Disable"));

    ui->test1->setCurrentText("Disable");
    ui->test2->setCurrentText("Disable");
    ui->test3->setCurrentText("Disable");
    ui->test4->setCurrentText("Disable");
    ui->test5->setCurrentText("Disable");
    ui->test6->setCurrentText("Disable");
    ui->test7->setCurrentText("Disable");
    ui->test8->setCurrentText("Disable");
    ui->test9->setCurrentText("Disable");
    ui->test10->setCurrentText("Disable");
    ui->test11->setCurrentText("Disable");
    ui->test12->setCurrentText("Disable");
    ui->test13->setCurrentText("Disable");
    ui->test14->setCurrentText("Disable");
    ui->test15->setCurrentText("Disable");
    ui->test16->setCurrentText("Disable");
    ui->test17->setCurrentText("Disable");
    ui->test18->setCurrentText("Disable");

    qkdparam.QKD_setDefault();

    //dbc.start();

    initR=true;

    createQKDLinesA();
    createQKDLinesB();
    createQKDLinesC();

    this->LoadState("default.conf", false);

    this->DrawExpectedSignal();
}

//////////////////////////////////////////////////////////
///////////////////setups///////////////////////////
///////////////////////////////////////////////////////////

void MainWindow::blah()
{
    ui->PlotA->replot();
    ui->PlotA_2->replot();
    ui->PlotB->replot();
    ui->PlotB_2->replot();
    ui->PlotC->replot();
    ui->PlotC_2->replot();
}

void MainWindow::setup_histolines_QKD()
{
    for (int i = 0 ; i < MAX_QUBITS ; i++) {
        LinesPlotA1[i] = new QCPItemStraightLine(ui->PlotA);
        LinesPlotA1[i]->setPen(QPen(QColor(255,0,0,128)));
        LinesPlotA1[i]->setVisible(0);

        LinesPlotA2[i] = new QCPItemStraightLine(ui->PlotA);
        LinesPlotA2[i]->setPen(QPen(QColor(0,255,0,128)));
        LinesPlotA2[i]->setVisible(0);

        LinesPlotA3[i] = new QCPItemStraightLine(ui->PlotA);
        LinesPlotA3[i]->setPen(QPen(Qt::yellow));
        LinesPlotA3[i]->setVisible(0);

        LinesPlotA4[i] = new QCPItemStraightLine(ui->PlotA);
        LinesPlotA4[i]->setPen(QPen(Qt::white));
        LinesPlotA4[i]->setVisible(0);

        LinesPlotB1[i] = new QCPItemStraightLine(ui->PlotB);
        LinesPlotB1[i]->setPen(QPen(QColor(255,255,255,128)));
        LinesPlotB1[i]->setVisible(0);

        LinesPlotB2[i] = new QCPItemStraightLine(ui->PlotB);
        LinesPlotB2[i]->setPen(QPen(QColor(255,0,0,128)));
        LinesPlotB2[i]->setVisible(0);

        LinesPlotB3[i] = new QCPItemStraightLine(ui->PlotB);
        LinesPlotB3[i]->setPen(QPen(QColor(255,255,255,128)));
        LinesPlotB3[i]->setVisible(0);

        LinesPlotB4[i] = new QCPItemStraightLine(ui->PlotB);
        LinesPlotB4[i]->setPen(QPen(Qt::white));
        LinesPlotB4[i]->setVisible(0);

        LinesPlotC1[i] = new QCPItemStraightLine(ui->PlotC);
        LinesPlotC1[i]->setPen(QPen(QColor(255,255,255,128)));
        LinesPlotC1[i]->setVisible(0);

        LinesPlotC2[i] = new QCPItemStraightLine(ui->PlotC);
        LinesPlotC2[i]->setPen(QPen(QColor(255,0,0,128)));
        LinesPlotC2[i]->setVisible(0);

        LinesPlotC3[i] = new QCPItemStraightLine(ui->PlotC);
        LinesPlotC3[i]->setPen(QPen(QColor(255,255,255,128)));
        LinesPlotC3[i]->setVisible(0);

        LinesPlotC4[i] = new QCPItemStraightLine(ui->PlotC);
        LinesPlotC4[i]->setPen(QPen(Qt::white));
        LinesPlotC4[i]->setVisible(0);
    }
}

void MainWindow::setupHistoPlot(QCustomPlot *histograma, bool top, bool yaxis)
{
    histograma->plotLayout()->clear();

    QCPAxisRect *wideAxisRect = new QCPAxisRect(histograma);

    wideAxisRect->setupFullAxesBox(true);
    if (yaxis)
        wideAxisRect->axis(QCPAxis::atRight, 0)->setTickLabels(true);
    else
        wideAxisRect->axis(QCPAxis::atLeft, 0)->setTickLabels(false);

    if (top)
        wideAxisRect->axis(QCPAxis::atTop, 0)->setTickLabels(true);

    wideAxisRect->axis(QCPAxis::atRight, 0)->setTickLabelColor(Qt::white);
    if (top)
        wideAxisRect->axis(QCPAxis::atTop, 0)->setTickLabelColor(Qt::white);
    wideAxisRect->axis(QCPAxis::atLeft, 0)->setTickLabelColor(Qt::white);
    wideAxisRect->axis(QCPAxis::atBottom, 0)->setTickLabelColor(Qt::white);

    wideAxisRect->axis(QCPAxis::atRight, 0)->setBasePen(QPen(Qt::white, 1));
    if (top)
        wideAxisRect->axis(QCPAxis::atTop, 0)->setBasePen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atLeft, 0)->setBasePen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atBottom, 0)->setBasePen(QPen(Qt::white, 1));

    wideAxisRect->axis(QCPAxis::atRight, 0)->setTickPen(QPen(Qt::white, 1));
    if (top)
        wideAxisRect->axis(QCPAxis::atTop, 0)->setTickPen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atLeft, 0)->setTickPen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atBottom, 0)->setTickPen(QPen(Qt::white, 1));

    wideAxisRect->axis(QCPAxis::atRight, 0)->setSubTickPen(QPen(Qt::white, 1));
    if (top)
        wideAxisRect->axis(QCPAxis::atTop, 0)->setSubTickPen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atLeft, 0)->setSubTickPen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atBottom, 0)->setSubTickPen(QPen(Qt::white, 1));

    wideAxisRect->axis(QCPAxis::atLeft, 0)->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    wideAxisRect->axis(QCPAxis::atBottom, 0)->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    wideAxisRect->axis(QCPAxis::atLeft, 0)->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    wideAxisRect->axis(QCPAxis::atBottom, 0)->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    wideAxisRect->axis(QCPAxis::atLeft, 0)->grid()->setSubGridVisible(true);
    wideAxisRect->axis(QCPAxis::atBottom, 0)->grid()->setSubGridVisible(true);
    //wideAxisRect->axis(QCPAxis::atLeft, 0)->grid()->setVisible(false);//
    //wideAxisRect->axis(QCPAxis::atBottom, 0)->grid()->setVisible(false);//
    wideAxisRect->axis(QCPAxis::atLeft, 0)->grid()->setZeroLinePen(Qt::NoPen);
    wideAxisRect->axis(QCPAxis::atBottom, 0)->grid()->setZeroLinePen(Qt::NoPen);
    wideAxisRect->axis(QCPAxis::atLeft, 0)->setUpperEnding(QCPLineEnding::esSpikeArrow);
    wideAxisRect->axis(QCPAxis::atBottom, 0)->setUpperEnding(QCPLineEnding::esSpikeArrow);

    //wideAxisRect->axis(QCPAxis::atLeft, 0)->setLabel("Cuentas");
    //wideAxisRect->axis(QCPAxis::atBottom, 0)->setLabel("Energia");

    wideAxisRect->axis(QCPAxis::atLeft, 0)->setLabelColor(Qt::white);
    wideAxisRect->axis(QCPAxis::atBottom, 0)->setLabelColor(Qt::white);

    wideAxisRect->setRangeZoom(Qt::Horizontal);

    /*QCPPlotTitle *title1 = new QCPPlotTitle(histograma);
    title1->setText("histo");
    title1->setFont(QFont("sans", 12, QFont::Bold));
    title1->setTextColor(Qt::white);

    */

    //histograma->plotLayout()->addElement(0, 0, title1);
    histograma->plotLayout()->addElement(0, 0, wideAxisRect);

    QCPGraph *graph1 = histograma->addGraph(wideAxisRect->axis(QCPAxis::atBottom), wideAxisRect->axis(QCPAxis::atLeft));
    //QCPGraph *graph2 = histograma->addGraph(wideAxisRect->axis(QCPAxis::atBottom), wideAxisRect->axis(QCPAxis::atLeft));

    graph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1), QBrush(Qt::white),4));
    graph1->setPen(QPen(QColor(255, 255, 255, 100), 2));
    graph1->setLineStyle((QCPGraph::LineStyle)4);

    histograma->xAxis->setRange(0, 10000);

    //histograma->addGraph();
    //histograma->graph(0)->setPen(QPen(Qt::red));

    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    histograma->setBackground(plotGradient);
    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
    axisRectGradient.setColorAt(1, QColor(30, 30, 30));
    histograma->axisRect()->setBackground(axisRectGradient);
    histograma->axisRect()->setMinimumMargins(QMargins(50,0,50,0));

    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    histograma->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

void MainWindow::setup_plot_qkd_results(QCustomPlot *scope)
{
    scope->plotLayout()->clear();

    QCPAxisRect *wideAxisRect = new QCPAxisRect(scope);

    wideAxisRect->setupFullAxesBox(true);
    wideAxisRect->axis(QCPAxis::atRight, 0)->setTickLabels(true);
    //wideAxisRect->axis(QCPAxis::atTop, 0)->setTickLabels(true);

    wideAxisRect->axis(QCPAxis::atRight, 0)->setTickLabelColor(Qt::white);
    //wideAxisRect->axis(QCPAxis::atTop, 0)->setTickLabelColor(Qt::white);
    wideAxisRect->axis(QCPAxis::atLeft, 0)->setTickLabelColor(Qt::white);
    wideAxisRect->axis(QCPAxis::atBottom, 0)->setTickLabelColor(Qt::white);

    wideAxisRect->axis(QCPAxis::atRight, 0)->setBasePen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atTop, 0)->setBasePen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atLeft, 0)->setBasePen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atBottom, 0)->setBasePen(QPen(Qt::white, 1));

    wideAxisRect->axis(QCPAxis::atRight, 0)->setTickPen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atTop, 0)->setTickPen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atLeft, 0)->setTickPen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atBottom, 0)->setTickPen(QPen(Qt::white, 1));

    wideAxisRect->axis(QCPAxis::atRight, 0)->setSubTickPen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atTop, 0)->setSubTickPen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atLeft, 0)->setSubTickPen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atBottom, 0)->setSubTickPen(QPen(Qt::white, 1));

    wideAxisRect->axis(QCPAxis::atLeft, 0)->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    wideAxisRect->axis(QCPAxis::atBottom, 0)->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    wideAxisRect->axis(QCPAxis::atLeft, 0)->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    wideAxisRect->axis(QCPAxis::atBottom, 0)->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    wideAxisRect->axis(QCPAxis::atLeft, 0)->grid()->setSubGridVisible(true);
    wideAxisRect->axis(QCPAxis::atBottom, 0)->grid()->setSubGridVisible(true);
    //wideAxisRect->axis(QCPAxis::atLeft, 0)->grid()->setVisible(false);//
    //wideAxisRect->axis(QCPAxis::atBottom, 0)->grid()->setVisible(false);//
    wideAxisRect->axis(QCPAxis::atLeft, 0)->grid()->setZeroLinePen(Qt::NoPen);
    wideAxisRect->axis(QCPAxis::atBottom, 0)->grid()->setZeroLinePen(Qt::NoPen);
    wideAxisRect->axis(QCPAxis::atLeft, 0)->setUpperEnding(QCPLineEnding::esSpikeArrow);
    wideAxisRect->axis(QCPAxis::atBottom, 0)->setUpperEnding(QCPLineEnding::esSpikeArrow);

    wideAxisRect->axis(QCPAxis::atLeft, 0)->setLabel("Counts");
    wideAxisRect->axis(QCPAxis::atBottom, 0)->setLabel("Time");

    wideAxisRect->axis(QCPAxis::atLeft, 0)->setLabelColor(Qt::white);
    wideAxisRect->axis(QCPAxis::atBottom, 0)->setLabelColor(Qt::white);

    wideAxisRect->setRangeZoom(Qt::Vertical);

    //scope->plotLayout()->addElement(0, 0, title1);
    scope->plotLayout()->addElement(0, 0, wideAxisRect);

    QCPGraph *graph1 = scope->addGraph(wideAxisRect->axis(QCPAxis::atBottom), wideAxisRect->axis(QCPAxis::atLeft));
    graph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1), QBrush(Qt::green),4));
    graph1->setPen(QPen(Qt::green, 2));

    QCPGraph *graph2 = scope->addGraph(wideAxisRect->axis(QCPAxis::atBottom), wideAxisRect->axis(QCPAxis::atLeft));
    graph2->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1), QBrush(Qt::red),4));
    graph2->setPen(QPen(Qt::red, 2));

    QCPGraph *graph3 = scope->addGraph(wideAxisRect->axis(QCPAxis::atBottom), wideAxisRect->axis(QCPAxis::atLeft));
    graph3->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1), QBrush(Qt::black),4));
    graph3->setPen(QPen(Qt::white, 2));

    QCPGraph *graph4 = scope->addGraph(wideAxisRect->axis(QCPAxis::atBottom), wideAxisRect->axis(QCPAxis::atLeft));
    graph4->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1), QBrush(Qt::white),4));
    graph4->setPen(QPen(Qt::black, 2));

    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    scope->setBackground(plotGradient);
    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
    axisRectGradient.setColorAt(1, QColor(30, 30, 30));
    scope->axisRect()->setBackground(axisRectGradient);


    //scope->yAxis->setRange(0, 2);

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    scope->xAxis->setTicker(timeTicker);
    scope->rescaleAxes();
    scope->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

void MainWindow::setup_plot_qkd_stats(QCustomPlot *scope)
{
    scope->plotLayout()->clear();

    QCPAxisRect *wideAxisRect = new QCPAxisRect(scope);

    wideAxisRect->setupFullAxesBox(true);
    wideAxisRect->axis(QCPAxis::atRight, 0)->setTickLabels(true);
    //wideAxisRect->axis(QCPAxis::atTop, 0)->setTickLabels(true);

    wideAxisRect->axis(QCPAxis::atRight, 0)->setTickLabelColor(Qt::white);
    //wideAxisRect->axis(QCPAxis::atTop, 0)->setTickLabelColor(Qt::white);
    wideAxisRect->axis(QCPAxis::atLeft, 0)->setTickLabelColor(Qt::white);
    wideAxisRect->axis(QCPAxis::atBottom, 0)->setTickLabelColor(Qt::white);

    wideAxisRect->axis(QCPAxis::atRight, 0)->setBasePen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atTop, 0)->setBasePen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atLeft, 0)->setBasePen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atBottom, 0)->setBasePen(QPen(Qt::white, 1));

    wideAxisRect->axis(QCPAxis::atRight, 0)->setTickPen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atTop, 0)->setTickPen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atLeft, 0)->setTickPen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atBottom, 0)->setTickPen(QPen(Qt::white, 1));

    wideAxisRect->axis(QCPAxis::atRight, 0)->setSubTickPen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atTop, 0)->setSubTickPen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atLeft, 0)->setSubTickPen(QPen(Qt::white, 1));
    wideAxisRect->axis(QCPAxis::atBottom, 0)->setSubTickPen(QPen(Qt::white, 1));

    wideAxisRect->axis(QCPAxis::atLeft, 0)->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    wideAxisRect->axis(QCPAxis::atBottom, 0)->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    wideAxisRect->axis(QCPAxis::atLeft, 0)->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    wideAxisRect->axis(QCPAxis::atBottom, 0)->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    wideAxisRect->axis(QCPAxis::atLeft, 0)->grid()->setSubGridVisible(true);
    wideAxisRect->axis(QCPAxis::atBottom, 0)->grid()->setSubGridVisible(true);
    //wideAxisRect->axis(QCPAxis::atLeft, 0)->grid()->setVisible(false);//
    //wideAxisRect->axis(QCPAxis::atBottom, 0)->grid()->setVisible(false);//
    wideAxisRect->axis(QCPAxis::atLeft, 0)->grid()->setZeroLinePen(Qt::NoPen);
    wideAxisRect->axis(QCPAxis::atBottom, 0)->grid()->setZeroLinePen(Qt::NoPen);
    wideAxisRect->axis(QCPAxis::atLeft, 0)->setUpperEnding(QCPLineEnding::esSpikeArrow);
    wideAxisRect->axis(QCPAxis::atBottom, 0)->setUpperEnding(QCPLineEnding::esSpikeArrow);

    wideAxisRect->axis(QCPAxis::atLeft, 0)->setLabel("Counts");
    wideAxisRect->axis(QCPAxis::atBottom, 0)->setLabel("Time");

    wideAxisRect->axis(QCPAxis::atLeft, 0)->setLabelColor(Qt::white);
    wideAxisRect->axis(QCPAxis::atBottom, 0)->setLabelColor(Qt::white);

    wideAxisRect->setRangeZoom(Qt::Vertical);

    //scope->plotLayout()->addElement(0, 0, title1);
    scope->plotLayout()->addElement(0, 0, wideAxisRect);

    QCPGraph *graph1 = scope->addGraph(wideAxisRect->axis(QCPAxis::atBottom), wideAxisRect->axis(QCPAxis::atLeft));
    graph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1), QBrush(Qt::white),4));
    graph1->setPen(QPen(Qt::white, 2));

    QCPGraph *graph2 = scope->addGraph(wideAxisRect->axis(QCPAxis::atBottom), wideAxisRect->axis(QCPAxis::atLeft));
    graph2->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1), QBrush(Qt::red),4));
    graph2->setPen(QPen(Qt::red, 2));

    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    scope->setBackground(plotGradient);
    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
    axisRectGradient.setColorAt(1, QColor(30, 30, 30));
    scope->axisRect()->setBackground(axisRectGradient);

    //scope->yAxis->setRange(0, 2);

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    scope->xAxis->setTicker(timeTicker);
    scope->rescaleAxes();
    scope->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

void MainWindow::refreshButton()
{
    int i;

    std::vector<std::string> devices = s.check_for_devices();

    ui->menuConnect->clear();
    for (i = 0; i < (int) devices.size(); i++) {
        ui->menuConnect->addAction(devices[i].c_str());
    }
    ui->menuConnect->addSeparator();
    ui->menuConnect->addAction("Refresh");
}

void MainWindow::connectAction(QAction *action)
{
    std::string action_string = action->iconText().toStdString();

    if (!strcmp(action_string.c_str(),"Refresh")) {
        this->refreshButton();
    } else {
        if (debug)
            fprintf(stderr, "connecting to swabian\n");
        if (s.connect(action_string) == 0) {
            if (debug)
                fprintf(stderr, "successfully connected to swabian\n");
        } else {
            if (debug)
                fprintf(stderr, "failed to connect to swabian\n");
        }
    }
}

void MainWindow::setupsignalslot()
{
    /* To be able to send complex types in signals, the data types need to be
     * registered with QT. */
    qRegisterMetaType<vectorInt64>("vectorInt64");
    qRegisterMetaType<vectorInt32>("vectorInt32");
    qRegisterMetaType<vectorInt8>("vectorInt8");
    qRegisterMetaType<vectorDouble>("vectorDouble");
    qRegisterMetaType<boolvector2d>("boolvector2d");
    qRegisterMetaType<intvector>("intvector");

    /* Buttons on the Histogram tab. */

    QObject::connect(ui->startChan, SIGNAL(valueChanged(int)), this, SLOT(histogramChanged()));
    QObject::connect(ui->PlotAChn1, SIGNAL(valueChanged(int)), this, SLOT(histogramChanged()));
    QObject::connect(ui->PlotBChn1, SIGNAL(valueChanged(int)), this, SLOT(histogramChanged()));
    QObject::connect(ui->PlotCChn1, SIGNAL(valueChanged(int)), this, SLOT(histogramChanged()));
    QObject::connect(ui->bin_width, SIGNAL(valueChanged(int)), this, SLOT(histogramChanged()));
    QObject::connect(ui->adqtime, SIGNAL(valueChanged(double)), this, SLOT(histogramChanged()));

    /* Note: There should be a cleaner way to do this with loops, but I don't know how. */
    QObject::connect(ui->threshold1, SIGNAL(valueChanged(double)), this, SLOT(parametersChanged()));
    QObject::connect(ui->threshold2, SIGNAL(valueChanged(double)), this, SLOT(parametersChanged()));
    QObject::connect(ui->threshold3, SIGNAL(valueChanged(double)), this, SLOT(parametersChanged()));
    QObject::connect(ui->threshold4, SIGNAL(valueChanged(double)), this, SLOT(parametersChanged()));
    QObject::connect(ui->threshold5, SIGNAL(valueChanged(double)), this, SLOT(parametersChanged()));
    QObject::connect(ui->threshold6, SIGNAL(valueChanged(double)), this, SLOT(parametersChanged()));
    QObject::connect(ui->threshold7, SIGNAL(valueChanged(double)), this, SLOT(parametersChanged()));
    QObject::connect(ui->threshold8, SIGNAL(valueChanged(double)), this, SLOT(parametersChanged()));
    QObject::connect(ui->threshold9, SIGNAL(valueChanged(double)), this, SLOT(parametersChanged()));
    QObject::connect(ui->threshold10, SIGNAL(valueChanged(double)), this, SLOT(parametersChanged()));
    QObject::connect(ui->threshold11, SIGNAL(valueChanged(double)), this, SLOT(parametersChanged()));
    QObject::connect(ui->threshold12, SIGNAL(valueChanged(double)), this, SLOT(parametersChanged()));
    QObject::connect(ui->threshold13, SIGNAL(valueChanged(double)), this, SLOT(parametersChanged()));
    QObject::connect(ui->threshold14, SIGNAL(valueChanged(double)), this, SLOT(parametersChanged()));
    QObject::connect(ui->threshold15, SIGNAL(valueChanged(double)), this, SLOT(parametersChanged()));
    QObject::connect(ui->threshold16, SIGNAL(valueChanged(double)), this, SLOT(parametersChanged()));
    QObject::connect(ui->threshold17, SIGNAL(valueChanged(double)), this, SLOT(parametersChanged()));
    QObject::connect(ui->threshold18, SIGNAL(valueChanged(double)), this, SLOT(parametersChanged()));

    QObject::connect(ui->delay1, SIGNAL(valueChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->delay2, SIGNAL(valueChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->delay3, SIGNAL(valueChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->delay4, SIGNAL(valueChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->delay5, SIGNAL(valueChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->delay6, SIGNAL(valueChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->delay7, SIGNAL(valueChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->delay8, SIGNAL(valueChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->delay9, SIGNAL(valueChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->delay10, SIGNAL(valueChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->delay11, SIGNAL(valueChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->delay12, SIGNAL(valueChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->delay13, SIGNAL(valueChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->delay14, SIGNAL(valueChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->delay15, SIGNAL(valueChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->delay16, SIGNAL(valueChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->delay17, SIGNAL(valueChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->delay18, SIGNAL(valueChanged(int)), this, SLOT(parametersChanged()));

    QObject::connect(ui->rof1, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->rof2, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->rof3, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->rof4, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->rof5, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->rof6, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->rof7, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->rof8, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->rof9, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->rof10, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->rof11, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->rof12, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->rof13, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->rof14, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->rof15, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->rof16, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->rof17, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->rof18, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));

    QObject::connect(ui->test1, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->test2, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->test3, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->test4, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->test5, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->test6, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->test7, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->test8, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->test9, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->test10, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->test11, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->test12, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->test13, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->test14, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->test15, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->test16, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->test17, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    QObject::connect(ui->test18, SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));

    /* Buttons on the Histogram tab. */

    QObject::connect(ui->actionSave_state, SIGNAL(triggered(bool)), this, SLOT(SaveStateDialog()));
    QObject::connect(ui->actionLoad_state, SIGNAL(triggered(bool)), this, SLOT(LoadStateDialog()));

    QObject::connect(this, SIGNAL(main_SaveAndValues(int, int, int , int , int, int , float , int )), &dbc, SLOT(SaveAndValues(int, int, int , int , int, int , float , int )));

    QObject::connect(this, SIGNAL(main_SaveRateValues( int, int , int , int , int , int , int , int , int , float)), &dbc, SLOT(SaveRateValues( int, int , int , int , int , int , int , int , int , float)));

    QObject::connect(ui->actionQKD, SIGNAL(triggered()), &qkdparam, SLOT(show()));

    QObject::connect(&qkdparam, SIGNAL(sig_QKD_timeA(double)), this, SLOT(chang_QKD_timeA(double)));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_timeB(double)), this, SLOT(chang_QKD_timeB(double)));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_timeC(double)), this, SLOT(chang_QKD_timeC(double)));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_numbA(int)), this, SLOT(chang_QKD_numbA(int)));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_numbB(int)), this, SLOT(chang_QKD_numbB(int)));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_numbC(int)), this, SLOT(chang_QKD_numbC(int)));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_phA(int)), this, SLOT(chang_QKD_phA(int)));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_phB(int)), this, SLOT(chang_QKD_phB(int)));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_phC(int)), this, SLOT(chang_QKD_phC(int)));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_iwA(int)), this, SLOT(chang_QKD_iwA(int)));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_iwB(int)), this, SLOT(chang_QKD_iwB(int)));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_iwC(int)), this, SLOT(chang_QKD_iwC(int)));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_zeroA(int)), this, SLOT(chang_QKD_zeroA(int)));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_zeroB(int)), this, SLOT(chang_QKD_zeroB(int)));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_zeroC(int)), this, SLOT(chang_QKD_zeroC(int)));

    QObject::connect(&qkdparam, SIGNAL(sig_QKD_timeA(double)), this, SLOT(DrawExpectedSignal()));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_timeB(double)), this, SLOT(DrawExpectedSignal()));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_timeC(double)), this, SLOT(DrawExpectedSignal()));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_numbA(int)), this, SLOT(DrawExpectedSignal()));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_numbB(int)), this, SLOT(DrawExpectedSignal()));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_numbC(int)), this, SLOT(DrawExpectedSignal()));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_phA(int)), this, SLOT(DrawExpectedSignal()));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_phB(int)), this, SLOT(DrawExpectedSignal()));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_phC(int)), this, SLOT(DrawExpectedSignal()));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_iwA(int)), this, SLOT(DrawExpectedSignal()));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_iwB(int)), this, SLOT(DrawExpectedSignal()));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_iwC(int)), this, SLOT(DrawExpectedSignal()));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_zeroA(int)), this, SLOT(DrawExpectedSignal()));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_zeroB(int)), this, SLOT(DrawExpectedSignal()));
    QObject::connect(&qkdparam, SIGNAL(sig_QKD_zeroC(int)), this, SLOT(DrawExpectedSignal()));

    QObject::connect(&dbc, SIGNAL(MYtables(QStringList)), this, SLOT(fillTablesNames(QStringList)));


    QObject::connect(&dbc, SIGNAL(qubitsfromDB(boolvector2d, int, int)), this, SLOT(set_qkd_datafromDB(boolvector2d , int, int)),Qt::QueuedConnection);

    QObject::connect(&qkdparam, SIGNAL(savehdf5()), this, SLOT(hdf5savefile()));
    QObject::connect(this, SIGNAL(MW_savehdf5(QString)), &dbc, SLOT(createHDF5forQKDdata(QString)));

    QObject::connect(this, SIGNAL(saveH5datafromMW(intvector,intvector,intvector,intvector,intvector,intvector,intvector,intvector,intvector,intvector,intvector,intvector)), &dbc, SLOT(appendQKDdata2HDF5(intvector,intvector,intvector,intvector,intvector,intvector,intvector,intvector,intvector,intvector,intvector,intvector)));

    QObject::connect(this, SIGNAL(main_SaveQKDresults(double,double, double, double ,double, double,double,double,double, double, double, double)),&dbc, SLOT(SaveQKDresults(double,double, double, double ,double, double,double,double,double, double, double, double)));
    QObject::connect(this, SIGNAL(main_SaveQKDstats(int, int, double, double)),&dbc, SLOT(SaveQKDstats(int, int, double, double)));

    QObject::connect(ui->menuLoad_Qubits, SIGNAL(triggered(QAction*)), this, SLOT(tableSelected(QAction*)));
    QObject::connect(ui->menuConnect, SIGNAL(triggered(QAction*)), this, SLOT(connectAction(QAction*)));
    QObject::connect(this , SIGNAL(tableQKDtoDB(QString)), &dbc, SLOT(readQubits(QString)));

    QObject::connect(&qkdparam, SIGNAL(sig_turnONDB(int)), this, SLOT(chang_QKD_turnONDB(int)));

    /* Set up the rate and histogram worker threads. */

    this->countWorkerThread = new CountWorkerThread(&this->s);
    connect(this->countWorkerThread, &CountWorkerThread::finished, this->countWorkerThread, &QObject::deleteLater);
    this->countWorkerThread->start();

    QObject::connect(this->countWorkerThread, &CountWorkerThread::rates_ready, this, &MainWindow::show_rates);

    int bin_width = ui->bin_width->value();
    timestamp_t time = static_cast<timestamp_t>(ui->adqtime->value()*1e12);
    int start_channel = ui->startChan->value();
    int chanA = ui->PlotAChn1->value();
    int chanB = ui->PlotBChn1->value();
    int chanC = ui->PlotCChn1->value();

    this->histogramWorkerThread = new HistogramWorkerThread(&this->s, start_channel, chanA, chanB, chanC, bin_width, time);
    connect(this->histogramWorkerThread, &HistogramWorkerThread::finished, this->histogramWorkerThread, &QObject::deleteLater);
    this->histogramWorkerThread->start();

    QObject::connect(this->histogramWorkerThread, &HistogramWorkerThread::histograms_ready, this, &MainWindow::show_histograms);

    this->histogramChanged();
    this->refreshButton();
}

void MainWindow::histogramChanged(void)
{
    int rof[18];

    int chanA = ui->PlotAChn1->value();
    int chanB = ui->PlotBChn1->value();
    int chanC = ui->PlotCChn1->value();

    this->histogramWorkerThread->bin_width = ui->bin_width->value();
    this->histogramWorkerThread->time = static_cast<timestamp_t>(ui->adqtime->value()*1e12);
    this->histogramWorkerThread->start_channel = ui->startChan->value();

    rof[0] = ui->rof1->currentIndex();
    rof[1] = ui->rof2->currentIndex();
    rof[2] = ui->rof3->currentIndex();
    rof[3] = ui->rof4->currentIndex();
    rof[4] = ui->rof5->currentIndex();
    rof[5] = ui->rof6->currentIndex();
    rof[6] = ui->rof7->currentIndex();
    rof[7] = ui->rof8->currentIndex();
    rof[8] = ui->rof9->currentIndex();
    rof[9] = ui->rof10->currentIndex();
    rof[10] = ui->rof11->currentIndex();
    rof[11] = ui->rof12->currentIndex();
    rof[12] = ui->rof13->currentIndex();
    rof[13] = ui->rof14->currentIndex();
    rof[14] = ui->rof15->currentIndex();
    rof[15] = ui->rof16->currentIndex();
    rof[16] = ui->rof17->currentIndex();
    rof[17] = ui->rof18->currentIndex();

    if (rof[chanA-1] == 0)
        this->histogramWorkerThread->chanA = chanA;
    else
        this->histogramWorkerThread->chanA = -chanA;
    if (rof[chanB-1] == 0)
        this->histogramWorkerThread->chanB = chanB;
    else
        this->histogramWorkerThread->chanB = -chanB;
    if (rof[chanC-1] == 0)
        this->histogramWorkerThread->chanC = chanC;
    else
        this->histogramWorkerThread->chanC = -chanC;
}

/* Plots the Swabian time difference histograms on the main Histogram tab. This
 * function is called when the histogram worker emits the histogram_ready
 * signal. */
void MainWindow::show_histograms(const vectorDouble &datA, const vectorDouble &datB, const vectorDouble &datC, int bin_width)
{
    int i, j;

    if (debug)
        fprintf(stderr, "show_histograms() called\n");

    double histEnd = ui->histEnd->value();

    QVector<double> xa(datA.size());
    QVector<double> ya(datA.size());

    if (debug)
        fprintf(stderr, "datA.size() = %i\n", datA.size());

    for (i = 0; i < datA.size()/2; i++) {
        if (xa.size() > 0 && xa.back() < datA[2*i] - bin_width) {
            xa.push_back(xa.back() - bin_width);
            ya.push_back(0);
        }
        if (datA[2*i] > histEnd) break;
        xa.push_back(datA[2*i]);
        ya.push_back(datA[2*i+1]);
        /* Hack to include empty bins. */
        if (i < datA.size()/2-1 && xa.back() + bin_width < datA[2*(i+1)]) {
            xa.push_back(xa.back() + bin_width);
            ya.push_back(0);
        }
    }

    ui->PlotA->graph(0)->data()->clear();
    // pass data points to graphs:
    ui->PlotA->graph(0)->setData(xa, ya);
    ui->PlotA->graph(0)->rescaleValueAxis();
    ui->PlotA->replot();

    QVector<double> xb(datB.size());
    QVector<double> yb(datB.size());

    if (debug)
        fprintf(stderr, "datB.size() = %i\n", datB.size());

    for (i = 0; i < datB.size()/2; i++) {
        if ((xb.size() > 0) && (xb.back() < datB[2*i] - bin_width)) {
            xb.push_back(xb.back() - bin_width);
            yb.push_back(0);
        }
        if (datB[2*i] > histEnd) break;
        xb.push_back(datB[2*i]);
        yb.push_back(datB[2*i+1]);
        /* Hack to include empty bins. */
        if ((i < datB.size()/2-1) && (xb.back() + bin_width < datB[2*(i+1)])) {
            xb.push_back(xb.back() + bin_width);
            yb.push_back(0);
        }
    }

    ui->PlotB->graph(0)->data()->clear();
    // pass data points to graphs:
    ui->PlotB->graph(0)->setData(xb, yb);
    ui->PlotB->graph(0)->rescaleValueAxis();
    ui->PlotB->replot();

    QVector<double> xc(datC.size());
    QVector<double> yc(datC.size());

    if (debug)
        fprintf(stderr, "datC.size() = %i\n", datC.size());

    for (i = 0; i < datC.size()/2; i++) {
        if (xc.size() > 0 && xc.back() < datC[2*i] - bin_width) {
            xc.push_back(xc.back() - bin_width);
            yc.push_back(0);
        }
        if (datC[2*i] > histEnd) break;
        xc.push_back(datC[2*i]);
        yc.push_back(datC[2*i+1]);
        /* Hack to include empty bins. */
        if (i < datC.size()/2-1 && xc.back() + bin_width < datC[2*(i+1)]) {
            xc.push_back(xc.back() + bin_width);
            yc.push_back(0);
        }
    }

    ui->PlotC->graph(0)->data()->clear();
    // pass data points to graphs:
    ui->PlotC->graph(0)->setData(xc, yc);
    ui->PlotC->graph(0)->rescaleValueAxis();
    ui->PlotC->replot();

    double resultAok=0, resultAerr=0, resultArand=0, resultAbkgnd=0;
    double resultBok=0, resultBerr=0, resultBrand=0, resultBbkgnd=0;
    double resultCok=0, resultCerr=0, resultCrand=0, resultCbkgnd=0;

    int nA, nB, nC;
    
    nA = MIN(in_QKD_numbA,(int) strlen(qubit_sequence));
    nB = MIN(in_QKD_numbB,(int) strlen(qubit_sequence));
    nC = MIN(in_QKD_numbC,(int) strlen(qubit_sequence));

    double resultEok=0, resultEerr=0, resultErand=0;
    double resultLok=0, resultLerr=0, resultLrand=0;
    double resultPok=0, resultPerr=0, resultPrand=0;
    double totalBkgnd=0;

    if (debug)
        fprintf(stderr, "computing stats from %i qubits\n", nA);

    QVector<double> xa_expected;
    QVector<double> ya_expected;

    for (j = 0; j < nA; j++) {
        double left, right;
        switch (qubit_sequence[j]) {
        case 'E':
            left = j*in_QKD_timeA + in_QKD_zeroA;
            right = left + in_QKD_iwA;
            xa_expected.push_back(left-0.1);
            ya_expected.push_back(0);
            xa_expected.push_back(left+0.1);
            ya_expected.push_back(1);
            xa_expected.push_back(right-0.1);
            ya_expected.push_back(1);
            xa_expected.push_back(right+0.1);
            ya_expected.push_back(0);
            break;
        case 'L':
            left = j*in_QKD_timeA + in_QKD_zeroA + in_QKD_phA;
            right = left + in_QKD_iwA;
            xa_expected.push_back(left-0.1);
            ya_expected.push_back(0);
            xa_expected.push_back(left+0.1);
            ya_expected.push_back(1);
            xa_expected.push_back(right-0.1);
            ya_expected.push_back(1);
            xa_expected.push_back(right+0.1);
            ya_expected.push_back(0);
            break;
        case '0':
            left = j*in_QKD_timeA + in_QKD_zeroA + in_QKD_phA;
            right = left + in_QKD_iwA;
            xa_expected.push_back(right+0.1);
            ya_expected.push_back(0);
            break;
        case 'P':
            left = j*in_QKD_timeA + in_QKD_zeroA;
            right = left + in_QKD_iwA;
            xa_expected.push_back(left-0.1);
            ya_expected.push_back(0);
            xa_expected.push_back(left+0.1);
            ya_expected.push_back(0.5);
            xa_expected.push_back(right-0.1);
            ya_expected.push_back(0.5);
            xa_expected.push_back(right+0.1);
            ya_expected.push_back(0);
            left = j*in_QKD_timeA + in_QKD_zeroA + in_QKD_phA;
            right = left + in_QKD_iwA;
            xa_expected.push_back(left-0.1);
            ya_expected.push_back(0);
            xa_expected.push_back(left+0.1);
            ya_expected.push_back(0.5);
            xa_expected.push_back(right-0.1);
            ya_expected.push_back(0.5);
            xa_expected.push_back(right+0.1);
            ya_expected.push_back(0);
            break;
        default:
            fprintf(stderr, "unknown qubit sequence character\n");
        }
    }

    ui->PlotA_2->graph(0)->data()->clear();
    // pass data points to graphs:
    ui->PlotA_2->graph(0)->setData(xa_expected, ya_expected);
    ui->PlotA_2->graph(0)->rescaleValueAxis();
    ui->PlotA_2->replot();

    /* FIXME: Definitely a better way to do this than a double for loop. */
    for (i = 0; i < datA.size()/2; i++) {
        if (datA[2*i] > histEnd) break;
        double t = datA[2*i];
        double count = datA[2*i+1];

        for (j = 0; j < nA; j++) {
            /* Find if we are in the early time bin. */
            double left = j*in_QKD_timeA + in_QKD_zeroA;
            double right = left + in_QKD_iwA;
            if ((t > left) && (t < right)) {
                /* Got an early signal. */
                switch (qubit_sequence[j]) {
                case 'E':
                    resultAok += count;
                    resultEok += count;
                    break;
                case 'L':
                    resultAerr += count;
                    resultEerr += count;
                    break;
                case '0':
                    resultAbkgnd += count;
                    totalBkgnd += count;
                    break;
                case 'P':
                    resultArand += count;
                    resultErand += count;
                    break;
                default:
                    fprintf(stderr, "unknown qubit sequence character\n");
                }
                break;
            }

            /* Find if we are in the late time bin. */
            left = j*in_QKD_timeA + in_QKD_zeroA + in_QKD_phA;
            right = left + in_QKD_iwA;
            if ((t > left) && (t < right)) {
                /* Got a late signal. */
                switch (qubit_sequence[j]) {
                case 'E':
                    resultAerr += count;
                    resultLerr += count;
                    break;
                case 'L':
                    resultAok += count;
                    resultLok += count;
                    break;
                case '0':
                    resultAbkgnd += count;
                    totalBkgnd += count;
                    break;
                case 'P':
                    resultArand += count;
                    resultLrand += count;
                    break;
                default:
                    fprintf(stderr, "unknown qubit sequence character\n");
                }
                break;
            }
        }
    }

    for (i = 0; i < datB.size()/2; i++) {
        if (datB[2*i] > histEnd) break;
        double t = datB[2*i];
        double count = datB[2*i+1];

        for (j = 0; j < nB; j++) {
            /* Find if we are in the early early time bin. */
            double left = j*in_QKD_timeB + in_QKD_zeroB;
            double right = left + in_QKD_iwB;
            if ((t > left) && (t < right)) {
                /* Got an early early signal, which is always ignored. */
                switch (qubit_sequence[j]) {
                case 'E':
                case 'L':
                case '0':
                case 'P':
                    resultBrand += count;
                    resultPrand += count;
                    break;
                default:
                    fprintf(stderr, "unknown qubit sequence character\n");
                }
                break;
            }

            /* Find if we are in the middle time bin. */
            left = j*in_QKD_timeB + in_QKD_zeroB + in_QKD_phB;
            right = left + in_QKD_iwB;
            if ((t > left) && (t < right)) {
                /* Got an early signal. */
                switch (qubit_sequence[j]) {
                case 'P':
                    resultBok += count;
                    resultPok += count;
                    break;
                case 'E':
                case 'L':
                    resultBrand += count;
                    resultPrand += count;
                    break;
                case '0':
                    resultBbkgnd += count;
                    totalBkgnd += count;
                    break;
                default:
                    fprintf(stderr, "unknown qubit sequence character\n");
                }
                break;
            }

            /* Find if we are in the late late time bin.
             * FIXME: handle overlap with previous bin. Right now we are double
             * counting. */
            left = j*in_QKD_timeB + in_QKD_zeroB + 2*in_QKD_phB;
            right = left + in_QKD_iwB;
            if ((t > left) && (t < right)) {
                /* Got a late late signal. */
                switch (qubit_sequence[j]) {
                case 'E':
                case 'L':
                case 'P':
                case '0':
                    resultBrand += count;
                    resultPrand += count;
                    break;
                default:
                    fprintf(stderr, "unknown qubit sequence character\n");
                }
                break;
            }
        }
    }
                
    for (i = 0; i < datC.size()/2; i++) {
        if (datC[2*i] > histEnd) break;
        double t = datC[2*i];
        double count = datC[2*i+1];

        for (j = 0; j < nC; j++) {
            /* Find if we are in the early early time bin. */
            double left = j*in_QKD_timeC + in_QKD_zeroC;
            double right = left + in_QKD_iwC;
            if ((t > left) && (t < right)) {
                /* Got an early early signal, which is always ignored. */
                switch (qubit_sequence[j]) {
                case 'E':
                case 'L':
                case '0':
                case 'P':
                    resultCrand += count;
                    break;
                default:
                    fprintf(stderr, "unknown qubit sequence character\n");
                }
                break;
            }

            /* Find if we are in the middle time bin. */
            left = j*in_QKD_timeC + in_QKD_zeroC + in_QKD_phC;
            right = left + in_QKD_iwC;
            if ((t > left) && (t < right)) {
                /* Got a middle signal. */
                switch (qubit_sequence[j]) {
                case 'P':
                    resultCerr += count;
                    resultPerr += count;
                    break;
                case 'E':
                case 'L':
                    resultCrand += count;
                    break;
                case '0':
                    resultCbkgnd += count;
                    totalBkgnd += count;
                    break;
                default:
                    fprintf(stderr, "unknown qubit sequence character\n");
                }
                break;
            }

            /* Find if we are in the late late time bin.
             * FIXME: handle overlap with previous bin. Right now we are double
             * counting. */
            left = j*in_QKD_timeC + in_QKD_zeroC + 2*in_QKD_phC;
            right = left + in_QKD_iwC;
            if ((t > left) && (t < right)) {
                /* Got a late late signal. */
                switch (qubit_sequence[j]) {
                case 'E':
                case 'L':
                case 'P':
                case '0':
                    resultCrand += count;
                    break;
                default:
                    fprintf(stderr, "unknown qubit sequence character\n");
                }
                break;
            }
        }
    }
                
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;

    plot_qkd_results_det(resultAok, resultAerr, resultArand, resultAbkgnd, resultBok, resultBerr, resultBrand, resultBbkgnd, resultCok, resultCerr, resultCrand, resultCbkgnd, key);
    plot_qkd_results_QB(resultEok, resultEerr, resultErand, totalBkgnd, resultLok, resultLerr, resultLrand, totalBkgnd, resultPok, resultPerr, resultPrand, totalBkgnd, key);

    if (debug) {
        printf("resultArand = %f\n", resultArand);
        printf("resultBok = %f\n", resultBok);
        printf("resultBerr = %f\n", resultBerr);
        printf("resultBrand = %f\n", resultBrand);
        printf("resultBbkgnd = %f\n", resultBbkgnd);
        printf("resultCok = %f\n", resultCok);
        printf("resultCerr = %f\n", resultCerr);
    }

    double sifted_time = resultAok/((double) resultAok+resultAerr);
    double sifted_phase = resultBok/((double) resultBok+resultBerr+resultCerr);
    double error_time = resultAerr/((double) resultAok+resultAerr);
    double error_phase = resultCerr/((double) resultBok+resultBerr+resultCerr);
    plot_qkd_stats(sifted_time, sifted_phase, error_time, error_phase, key);

    if (debug)
        fprintf(stderr, "show_histograms() done\n");
}

void MainWindow::DrawExpectedSignal(void)
{
    int j;

    int nA, nB, nC;
    
    nA = MIN(in_QKD_numbA,(int) strlen(qubit_sequence));
    nB = MIN(in_QKD_numbB,(int) strlen(qubit_sequence));
    nC = MIN(in_QKD_numbC,(int) strlen(qubit_sequence));

    QVector<double> xa_expected;
    QVector<double> ya_expected;

    for (j = 0; j < nA; j++) {
        double left, right;
        switch (qubit_sequence[j]) {
        case 'E':
            left = j*in_QKD_timeA + in_QKD_zeroA;
            right = left + in_QKD_iwA;
            xa_expected.push_back(left-0.1);
            ya_expected.push_back(0);
            xa_expected.push_back(left+0.1);
            ya_expected.push_back(1);
            xa_expected.push_back(right-0.1);
            ya_expected.push_back(1);
            xa_expected.push_back(right+0.1);
            ya_expected.push_back(0);
            break;
        case 'L':
            left = j*in_QKD_timeA + in_QKD_zeroA + in_QKD_phA;
            right = left + in_QKD_iwA;
            xa_expected.push_back(left-0.1);
            ya_expected.push_back(0);
            xa_expected.push_back(left+0.1);
            ya_expected.push_back(1);
            xa_expected.push_back(right-0.1);
            ya_expected.push_back(1);
            xa_expected.push_back(right+0.1);
            ya_expected.push_back(0);
            break;
        case '0':
            left = j*in_QKD_timeA + in_QKD_zeroA + in_QKD_phA;
            right = left + in_QKD_iwA;
            xa_expected.push_back(right+0.1);
            ya_expected.push_back(0);
            break;
        case 'P':
            left = j*in_QKD_timeA + in_QKD_zeroA;
            right = left + in_QKD_iwA;
            xa_expected.push_back(left-0.1);
            ya_expected.push_back(0);
            xa_expected.push_back(left+0.1);
            ya_expected.push_back(0.5);
            xa_expected.push_back(right-0.1);
            ya_expected.push_back(0.5);
            xa_expected.push_back(right+0.1);
            ya_expected.push_back(0);
            left = j*in_QKD_timeA + in_QKD_zeroA + in_QKD_phA;
            right = left + in_QKD_iwA;
            xa_expected.push_back(left-0.1);
            ya_expected.push_back(0);
            xa_expected.push_back(left+0.1);
            ya_expected.push_back(0.5);
            xa_expected.push_back(right-0.1);
            ya_expected.push_back(0.5);
            xa_expected.push_back(right+0.1);
            ya_expected.push_back(0);
            break;
        default:
            fprintf(stderr, "unknown qubit sequence character\n");
        }
    }

    QVector<double> xb_expected;
    QVector<double> yb_expected;

    for (j = 0; j < nB; j++) {
        double left, right;
        switch (qubit_sequence[j]) {
        case 'E':
            left = j*in_QKD_timeB + in_QKD_zeroB;
            right = left + in_QKD_iwB;
            xb_expected.push_back(left-0.1);
            yb_expected.push_back(0);
            xb_expected.push_back(left+0.1);
            yb_expected.push_back(0.5);
            xb_expected.push_back(right-0.1);
            yb_expected.push_back(0.5);
            xb_expected.push_back(right+0.1);
            yb_expected.push_back(0);
            left = j*in_QKD_timeB + in_QKD_zeroB + in_QKD_phB;
            right = left + in_QKD_iwB;
            xb_expected.push_back(left-0.1);
            yb_expected.push_back(0);
            xb_expected.push_back(left+0.1);
            yb_expected.push_back(0.5);
            xb_expected.push_back(right-0.1);
            yb_expected.push_back(0.5);
            xb_expected.push_back(right+0.1);
            yb_expected.push_back(0);
            break;
        case 'L':
            left = j*in_QKD_timeB + in_QKD_zeroB + in_QKD_phB;
            right = left + in_QKD_iwB;
            xb_expected.push_back(left-0.1);
            yb_expected.push_back(0);
            xb_expected.push_back(left+0.1);
            yb_expected.push_back(0.5);
            xb_expected.push_back(right-0.1);
            yb_expected.push_back(0.5);
            xb_expected.push_back(right+0.1);
            yb_expected.push_back(0);
            left = j*in_QKD_timeB + in_QKD_zeroB + 2*in_QKD_phB;
            right = left + in_QKD_iwB;
            xb_expected.push_back(left-0.1);
            yb_expected.push_back(0);
            xb_expected.push_back(left+0.1);
            yb_expected.push_back(0.5);
            xb_expected.push_back(right-0.1);
            yb_expected.push_back(0.5);
            xb_expected.push_back(right+0.1);
            yb_expected.push_back(0);
            break;
        case '0':
            left = j*in_QKD_timeB + in_QKD_zeroB + in_QKD_phB;
            right = left + in_QKD_iwB;
            xb_expected.push_back(right+0.1);
            yb_expected.push_back(0);
            break;
        case 'P':
            left = j*in_QKD_timeB + in_QKD_zeroB;
            right = left + in_QKD_iwB;
            xb_expected.push_back(left-0.1);
            yb_expected.push_back(0);
            xb_expected.push_back(left+0.1);
            yb_expected.push_back(0.25);
            xb_expected.push_back(right-0.1);
            yb_expected.push_back(0.25);
            xb_expected.push_back(right+0.1);
            yb_expected.push_back(0);
            left = j*in_QKD_timeB + in_QKD_zeroB + in_QKD_phB;
            right = left + in_QKD_iwB;
            xb_expected.push_back(left-0.1);
            yb_expected.push_back(0);
            xb_expected.push_back(left+0.1);
            yb_expected.push_back(0.5);
            xb_expected.push_back(right-0.1);
            yb_expected.push_back(0.5);
            xb_expected.push_back(right+0.1);
            yb_expected.push_back(0);
            left = j*in_QKD_timeB + in_QKD_zeroB + 2*in_QKD_phB;
            right = left + in_QKD_iwB;
            xb_expected.push_back(left-0.1);
            yb_expected.push_back(0);
            xb_expected.push_back(left+0.1);
            yb_expected.push_back(0.25);
            xb_expected.push_back(right-0.1);
            yb_expected.push_back(0.25);
            xb_expected.push_back(right+0.1);
            yb_expected.push_back(0);
            break;
        default:
            fprintf(stderr, "unknown qubit sequence character\n");
        }
    }

    QVector<double> xc_expected;
    QVector<double> yc_expected;

    for (j = 0; j < nC; j++) {
        double left, right;
        switch (qubit_sequence[j]) {
        case 'E':
            left = j*in_QKD_timeC + in_QKD_zeroC;
            right = left + in_QKD_iwC;
            xc_expected.push_back(left-0.1);
            yc_expected.push_back(0);
            xc_expected.push_back(left+0.1);
            yc_expected.push_back(0.5);
            xc_expected.push_back(right-0.1);
            yc_expected.push_back(0.5);
            xc_expected.push_back(right+0.1);
            yc_expected.push_back(0);
            left = j*in_QKD_timeC + in_QKD_zeroC + in_QKD_phC;
            right = left + in_QKD_iwC;
            xc_expected.push_back(left-0.1);
            yc_expected.push_back(0);
            xc_expected.push_back(left+0.1);
            yc_expected.push_back(0.5);
            xc_expected.push_back(right-0.1);
            yc_expected.push_back(0.5);
            xc_expected.push_back(right+0.1);
            yc_expected.push_back(0);
            break;
        case 'L':
            left = j*in_QKD_timeC + in_QKD_zeroC + in_QKD_phC;
            right = left + in_QKD_iwC;
            xc_expected.push_back(left-0.1);
            yc_expected.push_back(0);
            xc_expected.push_back(left+0.1);
            yc_expected.push_back(0.5);
            xc_expected.push_back(right-0.1);
            yc_expected.push_back(0.5);
            xc_expected.push_back(right+0.1);
            yc_expected.push_back(0);
            left = j*in_QKD_timeC + in_QKD_zeroC + 2*in_QKD_phC;
            right = left + in_QKD_iwC;
            xc_expected.push_back(left-0.1);
            yc_expected.push_back(0);
            xc_expected.push_back(left+0.1);
            yc_expected.push_back(0.5);
            xc_expected.push_back(right-0.1);
            yc_expected.push_back(0.5);
            xc_expected.push_back(right+0.1);
            yc_expected.push_back(0);
            break;
        case '0':
            left = j*in_QKD_timeC + in_QKD_zeroC + in_QKD_phC;
            right = left + in_QKD_iwC;
            xc_expected.push_back(right+0.1);
            yc_expected.push_back(0);
            break;
        case 'P':
            left = j*in_QKD_timeC + in_QKD_zeroC;
            right = left + in_QKD_iwC;
            xc_expected.push_back(left-0.1);
            yc_expected.push_back(0);
            xc_expected.push_back(left+0.1);
            yc_expected.push_back(0.25);
            xc_expected.push_back(right-0.1);
            yc_expected.push_back(0.25);
            xc_expected.push_back(right+0.1);
            yc_expected.push_back(0);
            left = j*in_QKD_timeC + in_QKD_zeroC + in_QKD_phC;
            right = left + in_QKD_iwC;
            xc_expected.push_back(left-0.1);
            yc_expected.push_back(0);
            xc_expected.push_back(left+0.1);
            yc_expected.push_back(0);
            xc_expected.push_back(right-0.1);
            yc_expected.push_back(0);
            xc_expected.push_back(right+0.1);
            yc_expected.push_back(0);
            left = j*in_QKD_timeC + in_QKD_zeroC + 2*in_QKD_phC;
            right = left + in_QKD_iwC;
            xc_expected.push_back(left-0.1);
            yc_expected.push_back(0);
            xc_expected.push_back(left+0.1);
            yc_expected.push_back(0.25);
            xc_expected.push_back(right-0.1);
            yc_expected.push_back(0.25);
            xc_expected.push_back(right+0.1);
            yc_expected.push_back(0);
            break;
        default:
            fprintf(stderr, "unknown qubit sequence character\n");
        }
    }

    ui->PlotA_2->graph(0)->data()->clear();
    //// pass data points to graphs:
    ui->PlotA_2->graph(0)->setData(xa_expected, ya_expected);
    ui->PlotA_2->yAxis->setRange(0, 1.5);
    ui->PlotA_2->replot();

    ui->PlotB_2->graph(0)->data()->clear();
    //// pass data points to graphs:
    ui->PlotB_2->graph(0)->setData(xb_expected, yb_expected);
    ui->PlotB_2->yAxis->setRange(0, 1.5);
    ui->PlotB_2->replot();

    ui->PlotC_2->graph(0)->data()->clear();
    //// pass data points to graphs:
    ui->PlotC_2->graph(0)->setData(xc_expected, yc_expected);
    ui->PlotC_2->yAxis->setRange(0, 1.5);
    ui->PlotC_2->replot();
}

/* Displays the event rate for each channel on the Parameters tab. This
 * function is called by the rates_ready signal from the count worker thread. */
void MainWindow::show_rates(double *rates)
{
    ui->rate1->display(rates[0]);
    ui->rate2->display(rates[1]);
    ui->rate3->display(rates[2]);
    ui->rate4->display(rates[3]);
    ui->rate5->display(rates[4]);
    ui->rate6->display(rates[5]);
    ui->rate7->display(rates[6]);
    ui->rate8->display(rates[7]);
    ui->rate9->display(rates[8]);
    ui->rate10->display(rates[9]);
    ui->rate11->display(rates[10]);
    ui->rate12->display(rates[11]);
    ui->rate13->display(rates[12]);
    ui->rate14->display(rates[13]);
    ui->rate15->display(rates[14]);
    ui->rate16->display(rates[15]);
    ui->rate17->display(rates[16]);
    ui->rate18->display(rates[17]);

    free(rates);
}

void MainWindow::parametersChanged(void)
{
    int i;
    double thresholds[18];
    double delay[18];
    int test[18];
    int rof[18];

    thresholds[0] = ui->threshold1->value();
    thresholds[1] = ui->threshold2->value();
    thresholds[2] = ui->threshold3->value();
    thresholds[3] = ui->threshold4->value();
    thresholds[4] = ui->threshold5->value();
    thresholds[5] = ui->threshold6->value();
    thresholds[6] = ui->threshold7->value();
    thresholds[7] = ui->threshold8->value();
    thresholds[8] = ui->threshold9->value();
    thresholds[9] = ui->threshold10->value();
    thresholds[10] = ui->threshold11->value();
    thresholds[11] = ui->threshold12->value();
    thresholds[12] = ui->threshold13->value();
    thresholds[13] = ui->threshold14->value();
    thresholds[14] = ui->threshold15->value();
    thresholds[15] = ui->threshold16->value();
    thresholds[16] = ui->threshold17->value();
    thresholds[17] = ui->threshold18->value();

    delay[0] = ui->delay1->value();
    delay[1] = ui->delay2->value();
    delay[2] = ui->delay3->value();
    delay[3] = ui->delay4->value();
    delay[4] = ui->delay5->value();
    delay[5] = ui->delay6->value();
    delay[6] = ui->delay7->value();
    delay[7] = ui->delay8->value();
    delay[8] = ui->delay9->value();
    delay[9] = ui->delay10->value();
    delay[10] = ui->delay11->value();
    delay[11] = ui->delay12->value();
    delay[12] = ui->delay13->value();
    delay[13] = ui->delay14->value();
    delay[14] = ui->delay15->value();
    delay[15] = ui->delay16->value();
    delay[16] = ui->delay17->value();
    delay[17] = ui->delay18->value();

    test[0] = ui->test1->currentIndex();
    test[1] = ui->test2->currentIndex();
    test[2] = ui->test3->currentIndex();
    test[3] = ui->test4->currentIndex();
    test[4] = ui->test5->currentIndex();
    test[5] = ui->test6->currentIndex();
    test[6] = ui->test7->currentIndex();
    test[7] = ui->test8->currentIndex();
    test[8] = ui->test9->currentIndex();
    test[9] = ui->test10->currentIndex();
    test[10] = ui->test11->currentIndex();
    test[11] = ui->test12->currentIndex();
    test[12] = ui->test13->currentIndex();
    test[13] = ui->test14->currentIndex();
    test[14] = ui->test15->currentIndex();
    test[15] = ui->test16->currentIndex();
    test[16] = ui->test17->currentIndex();
    test[17] = ui->test18->currentIndex();

    rof[0] = ui->rof1->currentIndex();
    rof[1] = ui->rof2->currentIndex();
    rof[2] = ui->rof3->currentIndex();
    rof[3] = ui->rof4->currentIndex();
    rof[4] = ui->rof5->currentIndex();
    rof[5] = ui->rof6->currentIndex();
    rof[6] = ui->rof7->currentIndex();
    rof[7] = ui->rof8->currentIndex();
    rof[8] = ui->rof9->currentIndex();
    rof[9] = ui->rof10->currentIndex();
    rof[10] = ui->rof11->currentIndex();
    rof[11] = ui->rof12->currentIndex();
    rof[12] = ui->rof13->currentIndex();
    rof[13] = ui->rof14->currentIndex();
    rof[14] = ui->rof15->currentIndex();
    rof[15] = ui->rof16->currentIndex();
    rof[16] = ui->rof17->currentIndex();
    rof[17] = ui->rof18->currentIndex();

    if (!s.t) {
        fprintf(stderr, "no time tagger connected\n");
        return;
    }

    int rising_channel_mask = 0;
    for (i = 0; i < MAX_CHANNELS; i++) {
        s.set_trigger_level(i+1,thresholds[i]);
        s.set_delay(i+1,delay[i]);
        s.set_test_signal(i+1,!test[i]);
        if (rof[i] == 0) {
            rising_channel_mask |= (1 << i);
        }
    }
    s.set_rising_mask(rising_channel_mask);

    /* Change the histogram because the trigger edge settings may have changed. */
    this->histogramChanged();
}

//////////////////////////////////////////////////////////
///////////////////plotting///////////////////////////
///////////////////////////////////////////////////////////

void MainWindow::plot_qkd_results_det(double okA, double errA, double randA, double bkgndA, double okB, double errB, double randB, double bkgndB, double okC, double errC, double randC, double bkgndC, double key)
{
    ui->QKD_H1_results->graph(0)->addData(key-lastPointKey_tab3, okA);
    ui->QKD_H1_results->graph(1)->addData(key-lastPointKey_tab3, errA);
    ui->QKD_H1_results->graph(2)->addData(key-lastPointKey_tab3, randA);
    ui->QKD_H1_results->graph(3)->addData(key-lastPointKey_tab3, bkgndA);

    ui->QKD_H2_results->graph(0)->addData(key-lastPointKey_tab3, okB);
    ui->QKD_H2_results->graph(1)->addData(key-lastPointKey_tab3, errB);
    ui->QKD_H2_results->graph(2)->addData(key-lastPointKey_tab3, randB);
    ui->QKD_H2_results->graph(3)->addData(key-lastPointKey_tab3, bkgndB);

    ui->QKD_H3_results->graph(0)->addData(key-lastPointKey_tab3, okC);
    ui->QKD_H3_results->graph(1)->addData(key-lastPointKey_tab3, errC);
    ui->QKD_H3_results->graph(2)->addData(key-lastPointKey_tab3, randC);
    ui->QKD_H3_results->graph(3)->addData(key-lastPointKey_tab3, bkgndC);

    ui->QKD_H1_results->xAxis->setRange(key-lastPointKey_tab1, 120, Qt::AlignRight);
    ui->QKD_H2_results->xAxis->setRange(key-lastPointKey_tab1, 120, Qt::AlignRight);
    ui->QKD_H3_results->xAxis->setRange(key-lastPointKey_tab1, 120, Qt::AlignRight);

    ui->QKD_H1_results->rescaleAxes();
    ui->QKD_H2_results->rescaleAxes();
    ui->QKD_H3_results->rescaleAxes();

    ui->QKD_H1_results->replot();
    ui->QKD_H2_results->replot();
    ui->QKD_H3_results->replot();
}

void MainWindow::plot_qkd_results_QB(double okE, double errE, double randE, double bkgndE, double okL, double errL, double randL, double bkgndL, double okP, double errP, double randP, double bkgndP, double key)
{
    ui->Early_results->graph(0)->addData(key-lastPointKey_tab3, okE);
    ui->Early_results->graph(1)->addData(key-lastPointKey_tab3, errE);
    ui->Early_results->graph(2)->addData(key-lastPointKey_tab3, randE);
    ui->Early_results->graph(3)->addData(key-lastPointKey_tab3, bkgndE);

    ui->Late_results->graph(0)->addData(key-lastPointKey_tab3, okL);
    ui->Late_results->graph(1)->addData(key-lastPointKey_tab3, errL);
    ui->Late_results->graph(2)->addData(key-lastPointKey_tab3, randL);
    ui->Late_results->graph(3)->addData(key-lastPointKey_tab3, bkgndL);

    ui->Phase_results->graph(0)->addData(key-lastPointKey_tab3, okP);
    ui->Phase_results->graph(1)->addData(key-lastPointKey_tab3, errP);
    ui->Phase_results->graph(2)->addData(key-lastPointKey_tab3, randP);
    ui->Phase_results->graph(3)->addData(key-lastPointKey_tab3, bkgndP);

    ui->Early_results->xAxis->setRange(key-lastPointKey_tab1, 120, Qt::AlignRight);
    ui->Late_results->xAxis->setRange(key-lastPointKey_tab1, 120, Qt::AlignRight);
    ui->Phase_results->xAxis->setRange(key-lastPointKey_tab1, 120, Qt::AlignRight);

    ui->Early_results->rescaleAxes();
    ui->Late_results->rescaleAxes();
    ui->Phase_results->rescaleAxes();

    ui->Early_results->replot();
    ui->Late_results->replot();
    ui->Phase_results->replot();
}

void MainWindow::plot_qkd_stats(double sifted_time, double sifted_phase, double error_time, double error_phase, double key)
{
    ui->qkd_errorplot->graph(0)->addData(key-lastPointKey_tab3, error_time);
    ui->qkd_errorplot->graph(1)->addData(key-lastPointKey_tab3, error_phase);

    ui->qkd_siftedplot->graph(0)->addData(key-lastPointKey_tab3, sifted_time);
    ui->qkd_siftedplot->graph(1)->addData(key-lastPointKey_tab3, sifted_phase);

    ui->qkd_errorplot->xAxis->setRange(key-lastPointKey_tab1, 120, Qt::AlignRight);
    ui->qkd_siftedplot->xAxis->setRange(key-lastPointKey_tab1, 120, Qt::AlignRight);

    ui->qkd_errorplot->rescaleAxes();
    ui->qkd_siftedplot->rescaleAxes();

    ui->qkd_errorplot->replot();
    ui->qkd_siftedplot->replot();
}

/////////////////////////////////////
///////////lines plots///////////////
/////////////////////////////////////


void MainWindow::createQKDLinesA()
{
    for (int i = 0; i < in_QKD_numbA*2; i++) {
       if ((i+1) % 2) {
            LinesPlotA1[i]->setVisible(1);
            LinesPlotA1[i]->point1->setCoords(i/2*in_QKD_timeA+in_QKD_zeroA,0);
            LinesPlotA1[i]->point2->setCoords(i/2*in_QKD_timeA+in_QKD_zeroA,1);
            LinesPlotA2[i]->setVisible(1);
            LinesPlotA2[i]->point1->setCoords(i/2*in_QKD_timeA+in_QKD_zeroA+in_QKD_phA,0);
            LinesPlotA2[i]->point2->setCoords(i/2*in_QKD_timeA+in_QKD_zeroA+in_QKD_phA,1);
            //LinesPlotA3[i]->setVisible(1);
            //LinesPlotA3[i]->point1->setCoords(i/2*in_QKD_timeA+in_QKD_zeroA+2*in_QKD_phA,0);
            //LinesPlotA3[i]->point2->setCoords(i/2*in_QKD_timeA+in_QKD_zeroA+2*in_QKD_phA,1);
            //LinesPlotA4[i]->setVisible(1);
            //LinesPlotA4[i]->point1->setCoords(i/2*in_QKD_timeA+in_QKD_zeroA+3*in_QKD_phA,0);
            //LinesPlotA4[i]->point2->setCoords(i/2*in_QKD_timeA+in_QKD_zeroA+3*in_QKD_phA,1);
       } else {
            LinesPlotA1[i]->setVisible(1);
            LinesPlotA1[i]->point1->setCoords((i-1)/2*in_QKD_timeA+in_QKD_zeroA+in_QKD_iwA,0);
            LinesPlotA1[i]->point2->setCoords((i-1)/2*in_QKD_timeA+in_QKD_zeroA+in_QKD_iwA,1);
            LinesPlotA2[i]->setVisible(1);
            LinesPlotA2[i]->point1->setCoords((i-1)/2*in_QKD_timeA+in_QKD_zeroA+in_QKD_iwA+in_QKD_phA,0);
            LinesPlotA2[i]->point2->setCoords((i-1)/2*in_QKD_timeA+in_QKD_zeroA+in_QKD_iwA+in_QKD_phA,1);
            //LinesPlotA3[i]->setVisible(1);
            //LinesPlotA3[i]->point1->setCoords((i-1)/2*in_QKD_timeA+in_QKD_zeroA+in_QKD_iwA+2*in_QKD_phA,0);
            //LinesPlotA3[i]->point2->setCoords((i-1)/2*in_QKD_timeA+in_QKD_zeroA+in_QKD_iwA+2*in_QKD_phA,1);
            //LinesPlotA4[i]->setVisible(1);
            //LinesPlotA4[i]->point1->setCoords((i-1)/2*in_QKD_timeA+in_QKD_zeroA+in_QKD_iwA+3*in_QKD_phA,0);
            //LinesPlotA4[i]->point2->setCoords((i-1)/2*in_QKD_timeA+in_QKD_zeroA+in_QKD_iwA+3*in_QKD_phA,1);
       }
    }

    ui->PlotA->replot();
}

void MainWindow::createQKDLinesB()
{
    for(int i=0; i < in_QKD_numbB*2; i++) {
       if ((i+1) % 2) {
            LinesPlotB1[i]->setVisible(1);
            LinesPlotB1[i]->point1->setCoords(i/2*in_QKD_timeB+in_QKD_zeroB,0);
            LinesPlotB1[i]->point2->setCoords(i/2*in_QKD_timeB+in_QKD_zeroB,1);
            LinesPlotB2[i]->setVisible(1);
            LinesPlotB2[i]->point1->setCoords(i/2*in_QKD_timeB+in_QKD_zeroB+in_QKD_phB,0);
            LinesPlotB2[i]->point2->setCoords(i/2*in_QKD_timeB+in_QKD_zeroB+in_QKD_phB,1);
            LinesPlotB3[i]->setVisible(1);
            LinesPlotB3[i]->point1->setCoords(i/2*in_QKD_timeB+in_QKD_zeroB+2*in_QKD_phB,0);
            LinesPlotB3[i]->point2->setCoords(i/2*in_QKD_timeB+in_QKD_zeroB+2*in_QKD_phB,1);
            //LinesPlotB4[i]->setVisible(1);
            //LinesPlotB4[i]->point1->setCoords(i/2*in_QKD_timeB+in_QKD_zeroB+3*in_QKD_phB,0);
            //LinesPlotB4[i]->point2->setCoords(i/2*in_QKD_timeB+in_QKD_zeroB+3*in_QKD_phB,1);
       } else {
            LinesPlotB1[i]->setVisible(1);
            LinesPlotB1[i]->point1->setCoords((i-1)/2*in_QKD_timeB+in_QKD_zeroB+in_QKD_iwB,0);
            LinesPlotB1[i]->point2->setCoords((i-1)/2*in_QKD_timeB+in_QKD_zeroB+in_QKD_iwB,1);
            LinesPlotB2[i]->setVisible(1);
            LinesPlotB2[i]->point1->setCoords((i-1)/2*in_QKD_timeB+in_QKD_zeroB+in_QKD_iwB+in_QKD_phB,0);
            LinesPlotB2[i]->point2->setCoords((i-1)/2*in_QKD_timeB+in_QKD_zeroB+in_QKD_iwB+in_QKD_phB,1);
            LinesPlotB3[i]->setVisible(1);
            LinesPlotB3[i]->point1->setCoords((i-1)/2*in_QKD_timeB+in_QKD_zeroB+in_QKD_iwB+2*in_QKD_phB,0);
            LinesPlotB3[i]->point2->setCoords((i-1)/2*in_QKD_timeB+in_QKD_zeroB+in_QKD_iwB+2*in_QKD_phB,1);
            //LinesPlotB4[i]->setVisible(1);
            //LinesPlotB4[i]->point1->setCoords((i-1)/2*in_QKD_timeB+in_QKD_zeroB+in_QKD_iwB+3*in_QKD_phB,0);
            //LinesPlotB4[i]->point2->setCoords((i-1)/2*in_QKD_timeB+in_QKD_zeroB+in_QKD_iwB+3*in_QKD_phB,1);
       }
    }

    ui->PlotB->replot();
}

void MainWindow::createQKDLinesC()
{
    for (int i=0; i < in_QKD_numbC*2; i++) {
       if ((i+1) % 2) {
            LinesPlotC1[i]->setVisible(1);
            LinesPlotC1[i]->point1->setCoords(i/2*in_QKD_timeC+in_QKD_zeroC,0);
            LinesPlotC1[i]->point2->setCoords(i/2*in_QKD_timeC+in_QKD_zeroC,1);
            LinesPlotC2[i]->setVisible(1);
            LinesPlotC2[i]->point1->setCoords(i/2*in_QKD_timeC+in_QKD_zeroC+in_QKD_phC,0);
            LinesPlotC2[i]->point2->setCoords(i/2*in_QKD_timeC+in_QKD_zeroC+in_QKD_phC,1);
            LinesPlotC3[i]->setVisible(1);
            LinesPlotC3[i]->point1->setCoords(i/2*in_QKD_timeC+in_QKD_zeroC+2*in_QKD_phC,0);
            LinesPlotC3[i]->point2->setCoords(i/2*in_QKD_timeC+in_QKD_zeroC+2*in_QKD_phC,1);
            //LinesPlotC4[i]->setVisible(1);
            //LinesPlotC4[i]->point1->setCoords(i/2*in_QKD_timeC+in_QKD_zeroC+3*in_QKD_phC,0);
            //LinesPlotC4[i]->point2->setCoords(i/2*in_QKD_timeC+in_QKD_zeroC+3*in_QKD_phC,1);
       } else {
            LinesPlotC1[i]->setVisible(1);
            LinesPlotC1[i]->point1->setCoords((i-1)/2*in_QKD_timeC+in_QKD_zeroC+in_QKD_iwC,0);
            LinesPlotC1[i]->point2->setCoords((i-1)/2*in_QKD_timeC+in_QKD_zeroC+in_QKD_iwC,1);
            LinesPlotC2[i]->setVisible(1);
            LinesPlotC2[i]->point1->setCoords((i-1)/2*in_QKD_timeC+in_QKD_zeroC+in_QKD_iwC+in_QKD_phC,0);
            LinesPlotC2[i]->point2->setCoords((i-1)/2*in_QKD_timeC+in_QKD_zeroC+in_QKD_iwC+in_QKD_phC,1);
            LinesPlotC3[i]->setVisible(1);
            LinesPlotC3[i]->point1->setCoords((i-1)/2*in_QKD_timeC+in_QKD_zeroC+in_QKD_iwC+2*in_QKD_phC,0);
            LinesPlotC3[i]->point2->setCoords((i-1)/2*in_QKD_timeC+in_QKD_zeroC+in_QKD_iwC+2*in_QKD_phC,1);
            //LinesPlotC4[i]->setVisible(1);
            //LinesPlotC4[i]->point1->setCoords((i-1)/2*in_QKD_timeC+in_QKD_zeroC+in_QKD_iwC+3*in_QKD_phC,0);
            //LinesPlotC4[i]->point2->setCoords((i-1)/2*in_QKD_timeC+in_QKD_zeroC+in_QKD_iwC+3*in_QKD_phC,1);
       }
    }

    ui->PlotC->replot();
}

void MainWindow::hidelinesA(int val)
{
    for (int i=2*val;i < 2*in_QKD_numbA; i++) {
         LinesPlotA1[i]->setVisible(0);
         LinesPlotA2[i]->setVisible(0);
         LinesPlotA3[i]->setVisible(0);
         LinesPlotA4[i]->setVisible(0);
    }
}

void MainWindow::hidelinesB(int val)
{
    for (int i=2*val;i < 2*in_QKD_numbB; i++) {
         LinesPlotB1[i]->setVisible(0);
         LinesPlotB2[i]->setVisible(0);
         LinesPlotB3[i]->setVisible(0);
         LinesPlotB4[i]->setVisible(0);
    }
}

void MainWindow::hidelinesC(int val)
{
    for (int i=2*val;i < 2*in_QKD_numbC; i++) {
         LinesPlotC1[i]->setVisible(0);
         LinesPlotC2[i]->setVisible(0);
         LinesPlotC3[i]->setVisible(0);
         LinesPlotC4[i]->setVisible(0);
    }
}

void MainWindow::turnONDB(int val)
{
    /*if(!dbc.isRunning() && dbrunning == 0 && val==1){
        dbc.run();
        dbrunning=val;
    }
    if(dbc.isRunning() && dbrunning == 1 && val==0){
        dbrunning = val;
        while(dbc.isRunning())usleep(100);
        dbc.~DBControl();
    }*/
    dbrunning=val;
}

void MainWindow::SaveStateDialog(void) {
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save Current Configuration"), "",
            tr("Configuration (*.conf);;All Files (*)"));

    if (fileName.isEmpty()) {
        return;
    } else {
        this->SaveState(fileName);
    }
}

void MainWindow::SaveState(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("Unable to open file"),
            file.errorString());
        return;
    }

    QDataStream out(&file);

    out.setVersion(QDataStream::Qt_4_5);
    QMap<QString, int> mapint;
    QMap<QString, double> mapdouble;
    QString localstring;

    mapint.insert("in_QKD_timeA", in_QKD_timeA);
    mapint.insert("in_QKD_zeroA", in_QKD_zeroA);
    mapint.insert("in_QKD_iwA", in_QKD_iwA);
    mapint.insert("in_QKD_phA", in_QKD_phA);
    mapint.insert("in_QKD_numA", in_QKD_numbA);
    mapint.insert("in_QKD_timeB", in_QKD_timeB);
    mapint.insert("in_QKD_zeroB", in_QKD_zeroB);
    mapint.insert("in_QKD_iwB", in_QKD_iwB);
    mapint.insert("in_QKD_phB", in_QKD_phB);
    mapint.insert("in_QKD_numB", in_QKD_numbB);
    mapint.insert("in_QKD_timeC", in_QKD_timeC);
    mapint.insert("in_QKD_zeroC", in_QKD_zeroC);
    mapint.insert("in_QKD_iwC", in_QKD_iwC);
    mapint.insert("in_QKD_phC", in_QKD_phC);
    mapint.insert("in_QKD_numC", in_QKD_numbC);
    mapint.insert("in_startChan",ui->startChan->value());
    mapint.insert("in_PlotACh1",ui->PlotAChn1->value());
    mapint.insert("in_PlotBCh1",ui->PlotBChn1->value());
    mapint.insert("in_PlotCCh1",ui->PlotCChn1->value());
    mapint.insert("in_bin_width",ui->bin_width->value());

    mapdouble.insert("in_adqtime", ui->adqtime->value());

    out << mapint;
    out << mapdouble;
}

void MainWindow::LoadStateDialog(void)
{
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Load Configuration"), "",
            tr("Configuration (*.conf);;All Files (*)"));

    if (fileName.isEmpty()) {
        return;
    } else {
        this->LoadState(fileName);
    }
}

void MainWindow::LoadState(QString fileName, bool warnDialog)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        if (warnDialog) {
            QMessageBox::information(this, tr("Unable to open file"),
                file.errorString());
        }
        return;
    }

    QMap<QString, int> mapintout;
    QMap<QString, double> mapdoubleout;
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_4_5);
    in >> mapintout;

    if (mapintout.contains("in_QKD_timeA"))
        qkdparam.ui->QKD_timeA->setValue(mapintout.value("in_QKD_timeA"));
    if (mapintout.contains("in_QKD_zeroA"))
        qkdparam.ui->QKD_zeroA->setValue(mapintout.value("in_QKD_zeroA"));
    if (mapintout.contains("in_QKD_iwA"))
        qkdparam.ui->QKD_iwA->setValue(mapintout.value("in_QKD_iwA"));
    if (mapintout.contains("in_QKD_phA"))
        qkdparam.ui->QKD_phA->setValue(mapintout.value("in_QKD_phA"));
    if (mapintout.contains("in_QKD_numA"))
        qkdparam.ui->QKD_numbA->setValue(mapintout.value("in_QKD_numA"));
    if (mapintout.contains("in_QKD_timeB"))
        qkdparam.ui->QKD_timeB->setValue(mapintout.value("in_QKD_timeB"));
    if (mapintout.contains("in_QKD_zeroB"))
        qkdparam.ui->QKD_zeroB->setValue(mapintout.value("in_QKD_zeroB"));
    if (mapintout.contains("in_QKD_iwB"))
        qkdparam.ui->QKD_iwB->setValue(mapintout.value("in_QKD_iwB"));
    if (mapintout.contains("in_QKD_phB"))
        qkdparam.ui->QKD_phB->setValue(mapintout.value("in_QKD_phB"));
    if (mapintout.contains("in_QKD_numB"))
        qkdparam.ui->QKD_numbB->setValue(mapintout.value("in_QKD_numB"));
    if (mapintout.contains("in_QKD_timeC"))
        qkdparam.ui->QKD_timeC->setValue(mapintout.value("in_QKD_timeC"));
    if (mapintout.contains("in_QKD_zeroC"))
        qkdparam.ui->QKD_zeroC->setValue(mapintout.value("in_QKD_zeroC"));
    if (mapintout.contains("in_QKD_iwC"))
        qkdparam.ui->QKD_iwC->setValue(mapintout.value("in_QKD_iwC"));
    if (mapintout.contains("in_QKD_phC"))
        qkdparam.ui->QKD_phC->setValue(mapintout.value("in_QKD_phC"));
    if (mapintout.contains("in_QKD_numC"))
        qkdparam.ui->QKD_numbC->setValue(mapintout.value("in_QKD_numC"));

    if (mapintout.contains("in_startChan"))
        ui->startChan->setValue(mapintout.value("in_startChan"));
    if (mapintout.contains("in_PlotACh1"))
        ui->PlotAChn1->setValue(mapintout.value("in_PlotACh1"));
    if (mapintout.contains("in_PlotBCh1"))
        ui->PlotBChn1->setValue(mapintout.value("in_PlotBCh1"));
    if (mapintout.contains("in_PlotCCh1"))
        ui->PlotCChn1->setValue(mapintout.value("in_PlotCCh1"));
    if (mapintout.contains("in_bin_width"))
        ui->bin_width->setValue(mapintout.value("in_bin_width"));

    if (mapdoubleout.contains("in_adqtime"))
        ui->adqtime->setValue(mapdoubleout.value("in_adqtime"));

    if (debug) {
        QMapIterator<QString,int>i(mapintout);
        while (i.hasNext()) {
            i.next();
            std::cout<< i.key().toStdString() <<  ": " << i.value() << std::endl;
        }
    }

    in >> mapdoubleout;

    if (debug) {
        QMapIterator<QString,double>j(mapdoubleout);
        while (j.hasNext()) {
            j.next();
            std::cout<< j.key().toStdString() <<  ": " << j.value() << std::endl;
        }
    }
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::hdf5savefile()
{
    bool ok;
    QString comentario = QInputDialog::getText(this, tr("Record DATA"),tr("Insert a name for HDF5 the file"), QLineEdit::Normal,QDir::home().dirName(), &ok);

    if (ok && !comentario.isEmpty()){
        HDF5File_created=true;
        emit MW_savehdf5(comentario);
    }

   /*    QString key = QDateTime::currentDateTime().toString("dd_MMM_yyyy_hh:mm" );
       key.append(".txt");

       data = new QFile(key);

       if (!data->open(QIODevice::WriteOnly | QIODevice::Text))return;

       QTextStream out(data);
       out<<"# "<<comentario<<"\n";
       out<<"Vch0\tVch1\tCch0\tCch1\tTime\n";
       recorddata=true;*/
}

void MainWindow::fillTablesNames(QStringList tables_names)
{
    if (tables_names.length() == 0) {
        QMessageBox::warning(this,
                             "Tables",
                             "There are no tables to display in the database",
                             QMessageBox::Ok);
    } else {
       /* ui->menuLoad_Qubits->addAction()
        ui->comboBox_table_name->addItems(tables_names);

        ui->comboBox_table_name->setEnabled(true);
        ui->comboBox_table_name->setFocus();*/

        for (int i = 0; i < tables_names.size(); ++i)ui->menuLoad_Qubits->addAction(tables_names.at(i));
                         //std::cout << tables_names.at(i).toLocal8Bit().constData() << std::endl;
    }
}

void MainWindow::set_qkd_datafromDB(const boolvector2d &dat,int qkdcolumns, int qkdrows)
{
   /* std::cout<<dat.size()<<std::endl;
    std::cout<<dat[0].size()<<std::endl;
   for (int i = 0;i<dat.size();i++) {
       for (int j =0;j<dat[i].size();j++) {
           std::cout<<dat[i][j];
       }
        std::cout<<std::endl;
    }*/
    this->in_qkdfromDB=dat;
    for (int i = 0;i<in_qkdfromDB.size();i++) {
        std::cout<<i<<" : ";
        for (int j =0;j<in_qkdfromDB[i].size();j++) {
            std::cout<<in_qkdfromDB[i][j];
        }
         std::cout<<std::endl;
    }
    this->in_qubnumindb=qkdrows;
    this->in_qkddbcolumns=qkdcolumns;
    qubitsfromDBloaded=true;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    fprintf(stderr, "saving last state to default.conf\n");
    this->SaveState("default.conf");

    this->countWorkerThread->requestInterruption();
    this->histogramWorkerThread->requestInterruption();

    fprintf(stderr, "waiting for worker threads to quit\n");
    while (this->countWorkerThread->isRunning() || this->histogramWorkerThread->isRunning() || dbc.isRunning())
        usleep(100);
    fprintf(stderr, "done waiting\n");

    usleep(1000);

    event->accept();
}
