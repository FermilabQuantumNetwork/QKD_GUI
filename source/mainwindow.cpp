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
#include "ps.h"
#include <sys/time.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

char qubit_sequence[100000] = "E0E0L0L0P0P0";

unsigned long long microtime(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000000 + tv.tv_usec;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    int i;

    pthread_mutex_init(&this->sync,NULL);

    ui->setupUi(this);

    enabled_mask = 0;

    rate_widgets[0] = ui->rate1;
    rate_widgets[1] = ui->rate2;
    rate_widgets[2] = ui->rate3;
    rate_widgets[3] = ui->rate4;
    rate_widgets[4] = ui->rate5;
    rate_widgets[5] = ui->rate6;
    rate_widgets[6] = ui->rate7;
    rate_widgets[7] = ui->rate8;
    rate_widgets[8] = ui->rate9;
    rate_widgets[9] = ui->rate10;
    rate_widgets[10] = ui->rate11;
    rate_widgets[11] = ui->rate12;
    rate_widgets[12] = ui->rate13;
    rate_widgets[13] = ui->rate14;
    rate_widgets[14] = ui->rate15;
    rate_widgets[15] = ui->rate16;
    rate_widgets[16] = ui->rate17;
    rate_widgets[17] = ui->rate18;

    threshold_widgets[0] = ui->threshold1;
    threshold_widgets[1] = ui->threshold2;
    threshold_widgets[2] = ui->threshold3;
    threshold_widgets[3] = ui->threshold4;
    threshold_widgets[4] = ui->threshold5;
    threshold_widgets[5] = ui->threshold6;
    threshold_widgets[6] = ui->threshold7;
    threshold_widgets[7] = ui->threshold8;
    threshold_widgets[8] = ui->threshold9;
    threshold_widgets[9] = ui->threshold10;
    threshold_widgets[10] = ui->threshold11;
    threshold_widgets[11] = ui->threshold12;
    threshold_widgets[12] = ui->threshold13;
    threshold_widgets[13] = ui->threshold14;
    threshold_widgets[14] = ui->threshold15;
    threshold_widgets[15] = ui->threshold16;
    threshold_widgets[16] = ui->threshold17;
    threshold_widgets[17] = ui->threshold18;

    delay_widgets[0] = ui->delay1;
    delay_widgets[1] = ui->delay2;
    delay_widgets[2] = ui->delay3;
    delay_widgets[3] = ui->delay4;
    delay_widgets[4] = ui->delay5;
    delay_widgets[5] = ui->delay6;
    delay_widgets[6] = ui->delay7;
    delay_widgets[7] = ui->delay8;
    delay_widgets[8] = ui->delay9;
    delay_widgets[9] = ui->delay10;
    delay_widgets[10] = ui->delay11;
    delay_widgets[11] = ui->delay12;
    delay_widgets[12] = ui->delay13;
    delay_widgets[13] = ui->delay14;
    delay_widgets[14] = ui->delay15;
    delay_widgets[15] = ui->delay16;
    delay_widgets[16] = ui->delay17;
    delay_widgets[17] = ui->delay18;

    rof_widgets[0] = ui->rof1;
    rof_widgets[1] = ui->rof2;
    rof_widgets[2] = ui->rof3;
    rof_widgets[3] = ui->rof4;
    rof_widgets[4] = ui->rof5;
    rof_widgets[5] = ui->rof6;
    rof_widgets[6] = ui->rof7;
    rof_widgets[7] = ui->rof8;
    rof_widgets[8] = ui->rof9;
    rof_widgets[9] = ui->rof10;
    rof_widgets[10] = ui->rof11;
    rof_widgets[11] = ui->rof12;
    rof_widgets[12] = ui->rof13;
    rof_widgets[13] = ui->rof14;
    rof_widgets[14] = ui->rof15;
    rof_widgets[15] = ui->rof16;
    rof_widgets[16] = ui->rof17;
    rof_widgets[17] = ui->rof18;

    test_widgets[0] = ui->test1;
    test_widgets[1] = ui->test2;
    test_widgets[2] = ui->test3;
    test_widgets[3] = ui->test4;
    test_widgets[4] = ui->test5;
    test_widgets[5] = ui->test6;
    test_widgets[6] = ui->test7;
    test_widgets[7] = ui->test8;
    test_widgets[8] = ui->test9;
    test_widgets[9] = ui->test10;
    test_widgets[10] = ui->test11;
    test_widgets[11] = ui->test12;
    test_widgets[12] = ui->test13;
    test_widgets[13] = ui->test14;
    test_widgets[14] = ui->test15;
    test_widgets[15] = ui->test16;
    test_widgets[16] = ui->test17;
    test_widgets[17] = ui->test18;

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
    connect(ui->PlotA->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(replot_histograms()));
    connect(ui->PlotA_2->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->PlotA->xAxis, SLOT(setRange(QCPRange)));
    connect(ui->PlotA_2->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(replot_histograms()));

    connect(ui->PlotB->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->PlotB_2->xAxis, SLOT(setRange(QCPRange)));
    connect(ui->PlotB->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(replot_histograms()));
    connect(ui->PlotB_2->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->PlotB->xAxis, SLOT(setRange(QCPRange)));
    connect(ui->PlotB_2->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(replot_histograms()));

    connect(ui->PlotC->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->PlotC_2->xAxis, SLOT(setRange(QCPRange)));
    connect(ui->PlotC->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(replot_histograms()));
    connect(ui->PlotC_2->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->PlotC->xAxis, SLOT(setRange(QCPRange)));
    connect(ui->PlotC_2->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(replot_histograms()));

    setup_plot_qkd_results(ui->QKD_H1_results);
    setup_plot_qkd_results(ui->QKD_H2_results);
    setup_plot_qkd_results(ui->QKD_H3_results);
    setup_plot_qkd_stats(ui->qkd_errorplot);
    setup_plot_voltage(ui->qkd_siftedplot);
    setup_plot_qkd_results(ui->Early_results);
    setup_plot_qkd_results(ui->Late_results);
    setup_plot_qkd_results(ui->Phase_results);

    setup_histolines_QKD();

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

    for (i = 0; i < 18; i++) {
        rate_widgets[i]->setSegmentStyle(QLCDNumber::Flat);
        test_widgets[i]->addItem(tr("Enable"));
        test_widgets[i]->addItem(tr("Disable"));
        rof_widgets[i]->addItem(tr("Rise"));
        rof_widgets[i]->addItem(tr("Fall"));
        threshold_widgets[i]->setValue(0);
        delay_widgets[i]->setValue(0);
        rof_widgets[i]->setCurrentText("Fall");
        test_widgets[i]->setCurrentText("Disable");
    }

    qkdparam.QKD_setDefault();

    //dbc.start();

    initR=true;

    createQKDLinesA();
    createQKDLinesB();
    createQKDLinesC();

    ui->qubit_sequence->setPlainText(QString::fromLocal8Bit(qubit_sequence));

    this->LoadState("default.conf", false);

    this->prev_startChan = ui->startChan->value();
    this->prev_chanA = ui->PlotAChn1->value();
    this->prev_chanB = ui->PlotBChn1->value();;
    this->prev_chanC = ui->PlotCChn1->value();;

    this->DrawExpectedSignal();

    ui->ps_connect_button->setEnabled(true);
    ui->ps_disconnect_button->setEnabled(false);
    ui->ps_start_button->setEnabled(false);
    ui->ps_stop_button->setEnabled(false);
}

//////////////////////////////////////////////////////////
///////////////////setups///////////////////////////
///////////////////////////////////////////////////////////

void MainWindow::replot_histograms()
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

void MainWindow::setup_plot_voltage(QCustomPlot *scope)
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

    wideAxisRect->axis(QCPAxis::atLeft, 0)->setLabel("Voltage");
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

    wideAxisRect->axis(QCPAxis::atLeft, 0)->setLabel("Error Rate");
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
        QMenu *menu = ui->menuConnect->addMenu(devices[i].c_str());
        menu->addAction("Standard");
        menu->addAction("HighResA");
        menu->addAction("HighResB");
        menu->addAction("HighResC");
        QObject::connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(connectAction(QAction*)));
    }
    ui->menuConnect->addSeparator();
    ui->menuConnect->addAction("Refresh");
    ui->menuConnect->addAction("Disconnect");
}

void MainWindow::refreshAction(QAction *action)
{
    std::string action_string = action->iconText().toStdString();

    if (!strcmp(action_string.c_str(),"Refresh"))
        this->refreshButton();
    else if (!strcmp(action_string.c_str(),"Disconnect"))
        this->disconnectAction();
}

void MainWindow::disconnectAction(void)
{

    Log(NOTICE, "disconnectAction: waiting for worker threads to quit");

    if (this->countWorkerThread) {
        this->countWorkerThread->requestInterruption();

        while (this->countWorkerThread->isRunning())
            usleep(100);
    }

    if (this->histogramWorkerThread) {
        this->histogramWorkerThread->requestInterruption();

        while (this->histogramWorkerThread->isRunning())
            usleep(100);
    }

    Log(NOTICE, "disconnectAction: done waiting");

    this->countWorkerThread = NULL;
    this->histogramWorkerThread = NULL;

    this->s.disconnect();
    this->enabled_mask = 0;
    this->parametersChanged();
    this->histogramChanged();
}

void MainWindow::connectAction(QAction *action)
{
    std::string action_string = action->iconText().toStdString();
    QMenu *menu = (QMenu *) action->parentWidget();
    std::string device_string = menu->title().toStdString().c_str();

    Log(DEBUG, "action string = %s", action_string.c_str());
    Log(DEBUG, "device string = %s", menu->title().toStdString().c_str());

    Resolution resolution = Resolution::Standard;

    if (!strcmp(action_string.c_str(),"Standard"))
        resolution = Resolution::Standard;
    else if (!strcmp(action_string.c_str(),"HighResA"))
        resolution = Resolution::HighResA;
    else if (!strcmp(action_string.c_str(),"HighResB"))
        resolution = Resolution::HighResB;
    else if (!strcmp(action_string.c_str(),"HighResC"))
        resolution = Resolution::HighResC;
    else
        Log(WARNING, "unknown resolution '%s'", action_string.c_str());

    Log(NOTICE, "connecting to swabian");

    /* If we are already connected, make sure to disconnect first. */
    if (s.t)
        this->disconnectAction();

    if (s.connect(device_string, resolution) == 0) {
        Log(NOTICE, "successfully connected to swabian");

        if (resolution == Resolution::Standard)
            this->enabled_mask = 0x1ff;
        else if (resolution == Resolution::HighResA)
            this->enabled_mask = 0x155;
        else if (resolution == Resolution::HighResB)
            this->enabled_mask = 0x11;
        else if (resolution == Resolution::HighResC)
            this->enabled_mask = 0x10;

        this->parametersChanged();
        this->histogramChanged();

        this->countWorkerThread = new CountWorkerThread(&this->s, this->enabled_mask);
        connect(this->countWorkerThread, &CountWorkerThread::finished, this->countWorkerThread, &QObject::deleteLater);
        this->countWorkerThread->start();

        QObject::connect(this->countWorkerThread, &CountWorkerThread::rates_ready, this, &MainWindow::show_rates);

        int bin_width = ui->bin_width->value();
        timestamp_t time = static_cast<timestamp_t>(ui->adqtime->value()*1e12);
        int start_channel = ui->startChan->value();
        int chanA = ui->PlotAChn1->value();
        int chanB = ui->PlotBChn1->value();
        int chanC = ui->PlotCChn1->value();

        this->histogramWorkerThread = new HistogramWorkerThread(&this->s, start_channel, chanA, chanB, chanC, bin_width, time, &this->sync);
        connect(this->histogramWorkerThread, &HistogramWorkerThread::finished, this->histogramWorkerThread, &QObject::deleteLater);
        this->histogramWorkerThread->start();

        QObject::connect(this->histogramWorkerThread, &HistogramWorkerThread::histograms_ready, this, &MainWindow::show_histograms);

    } else {
        Log(DEBUG, "failed to connect to swabian");
    }
}

void MainWindow::PowerSupplyStart(void)
{
    if (this->phaseStabilizationThread) {
        this->phaseStabilizationThread->running = 1;
        ui->ps_start_button->setEnabled(false);
        ui->ps_stop_button->setEnabled(true);
    }
}

void MainWindow::PowerSupplyStop(void)
{
    if (this->phaseStabilizationThread) {
        this->phaseStabilizationThread->running = 0;
        ui->ps_start_button->setEnabled(true);
        ui->ps_stop_button->setEnabled(false);
    }
}

void MainWindow::PowerSupplyDisconnect(void)
{
    Log(NOTICE, "disconnecting from power supply");

    if (this->phaseStabilizationThread) {
        this->phaseStabilizationThread->requestInterruption();

        while (this->phaseStabilizationThread->isRunning())
            usleep(100);

        this->phaseStabilizationThread = NULL;

        ui->ps_connect_button->setEnabled(true);
        ui->ps_disconnect_button->setEnabled(false);
        ui->ps_start_button->setEnabled(false);
        ui->ps_stop_button->setEnabled(false);
    }
}

void MainWindow::PowerSupplyConnect(void)
{
    char ip_address[1024];
    int port;

    strcpy(ip_address,ui->ps_ip_address->text().toLocal8Bit().data());
    port = ui->ps_port->value();

    Log(NOTICE, "connecting to %s on port %i", ip_address, port);

    if (ps)
        ps_free(ps);

    ps = ps_init(ip_address,port);

    if (ps_connect(ps)) {
        Log(WARNING, "failed to connect to %s on port %i", ip_address, port);
        ps_free(ps);
    }

    Log(NOTICE, "successfully connected to %s on port %i", ip_address, port);

    this->phaseStabilizationThread = new PhaseStabilizationThread(this->ps, &this->sync);
    connect(this->phaseStabilizationThread, &PhaseStabilizationThread::finished, this->phaseStabilizationThread, &QObject::deleteLater);
    this->phaseStabilizationThread->start();

    QObject::connect(this->phaseStabilizationThread, &PhaseStabilizationThread::voltage_changed, this, &MainWindow::plot_voltage);

    ui->ps_connect_button->setEnabled(false);
    ui->ps_disconnect_button->setEnabled(true);
    ui->ps_start_button->setEnabled(true);
    ui->ps_stop_button->setEnabled(false);
}

void MainWindow::setupsignalslot()
{
    int i;

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

    for (i = 0; i < 18; i++) {
        QObject::connect(threshold_widgets[i], SIGNAL(valueChanged(double)), this, SLOT(parametersChanged()));
        QObject::connect(delay_widgets[i], SIGNAL(valueChanged(int)), this, SLOT(parametersChanged()));
        QObject::connect(rof_widgets[i], SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
        QObject::connect(test_widgets[i], SIGNAL(currentIndexChanged(int)), this, SLOT(parametersChanged()));
    }

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
    QObject::connect(ui->menuConnect, SIGNAL(triggered(QAction*)), this, SLOT(refreshAction(QAction*)));

    QObject::connect(this , SIGNAL(tableQKDtoDB(QString)), &dbc, SLOT(readQubits(QString)));

    QObject::connect(&qkdparam, SIGNAL(sig_turnONDB(int)), this, SLOT(chang_QKD_turnONDB(int)));

    /* Set up the rate and histogram worker threads. */

    this->countWorkerThread = NULL;
    this->histogramWorkerThread = NULL;
    this->phaseStabilizationThread = NULL;

    this->histogramChanged();
    this->refreshButton();

    QObject::connect(ui->ps_connect_button, &QPushButton::released, this, &MainWindow::PowerSupplyConnect);
    QObject::connect(ui->ps_disconnect_button, &QPushButton::released, this, &MainWindow::PowerSupplyDisconnect);
    QObject::connect(ui->ps_start_button, &QPushButton::released, this, &MainWindow::PowerSupplyStart);
    QObject::connect(ui->ps_stop_button, &QPushButton::released, this, &MainWindow::PowerSupplyStop);
    QObject::connect(ui->qubit_sequence, SIGNAL(textChanged()), this, SLOT(qubit_sequence_changed()));
}

void MainWindow::histogramChanged(void)
{
    int startChan = ui->startChan->value();
    int chanA = ui->PlotAChn1->value();
    int chanB = ui->PlotBChn1->value();
    int chanC = ui->PlotCChn1->value();

    if (enabled_mask == 0) {
        ui->startChan->setEnabled(false);
        ui->PlotAChn1->setEnabled(false);
        ui->PlotBChn1->setEnabled(false);
        ui->PlotCChn1->setEnabled(false);
        return;
    }

    ui->startChan->setEnabled(true);
    ui->PlotAChn1->setEnabled(true);
    ui->PlotBChn1->setEnabled(true);
    ui->PlotCChn1->setEnabled(true);

    Log(DEBUG, "validating start channel. new value = %i", startChan);

    /* Validate channels. */
    double diff = prev_startChan < startChan ? 1 : -1;
    while ((enabled_mask & (1 << (startChan-1))) == 0) {
        startChan += diff;
        if (startChan > 18) startChan = 0;
        if (startChan < 0) startChan = 18;
    }

    diff = prev_chanA < chanA ? 1 : -1;
    while ((enabled_mask & (1 << (chanA-1))) == 0) {
        chanA += diff;
        if (chanA > 18) chanA = 0;
        if (chanA < 0) chanA = 18;
    }

    diff = prev_chanB < chanB ? 1 : -1;
    while ((enabled_mask & (1 << (chanB-1))) == 0) {
        chanB += diff;
        if (chanB > 18) chanB = 0;
        if (chanB < 0) chanB = 18;
    }

    diff = prev_chanC < chanC ? 1 : -1;
    while ((enabled_mask & (1 << (chanC-1))) == 0) {
        chanC += diff;
        if (chanC > 18) chanC = 0;
        if (chanC < 0) chanC = 18;
    }

    ui->startChan->setValue(startChan);
    ui->PlotAChn1->setValue(chanA);
    ui->PlotBChn1->setValue(chanB);
    ui->PlotCChn1->setValue(chanC);

    prev_startChan = startChan;
    prev_chanA = chanA;
    prev_chanB = chanB;
    prev_chanC = chanC;

    if (this->histogramWorkerThread) {
        this->histogramWorkerThread->bin_width = ui->bin_width->value();
        this->histogramWorkerThread->time = static_cast<timestamp_t>(ui->adqtime->value()*1e12);
        this->histogramWorkerThread->start_channel = ui->startChan->value();

        this->histogramWorkerThread->chanA = chanA;
        this->histogramWorkerThread->chanB = chanB;
        this->histogramWorkerThread->chanC = chanC;
    }
}

void MainWindow::qubit_sequence_changed(void)
{
    strcpy(qubit_sequence,ui->qubit_sequence->toPlainText().toLocal8Bit().data());
}

/* Plots the Swabian time difference histograms on the main Histogram tab. This
 * function is called when the histogram worker emits the histogram_ready
 * signal. */
void MainWindow::show_histograms(const vectorDouble &datA, const vectorDouble &datB, const vectorDouble &datC, int bin_width, unsigned long long start, unsigned long long stop)
{
    int i, j;

    Log(DEBUG, "show_histograms() called");

    double histEnd = ui->histEnd->value();

    QVector<double> xa(datA.size());
    QVector<double> ya(datA.size());

    Log(DEBUG, "datA.size() = %i", datA.size());

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

    Log(DEBUG, "datB.size() = %i", datB.size());

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

    Log(DEBUG, "datC.size() = %i", datC.size());

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

    Log(DEBUG, "computing stats from %i qubits", nA);

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
            Log(WARNING, "unknown qubit sequence character");
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
                    Log(WARNING, "unknown qubit sequence character");
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
                    Log(WARNING, "unknown qubit sequence character");
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
                    Log(WARNING, "unknown qubit sequence character");
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
                    Log(WARNING, "unknown qubit sequence character");
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
                    Log(WARNING, "unknown qubit sequence character");
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
                    Log(WARNING, "unknown qubit sequence character");
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
                    Log(WARNING, "unknown qubit sequence character");
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
                    Log(WARNING, "unknown qubit sequence character");
                }
                break;
            }
        }
    }
                
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;

    plot_qkd_results_det(resultAok, resultAerr, resultArand, resultAbkgnd, resultBok, resultBerr, resultBrand, resultBbkgnd, resultCok, resultCerr, resultCrand, resultCbkgnd, key);
    plot_qkd_results_QB(resultEok, resultEerr, resultErand, totalBkgnd, resultLok, resultLerr, resultLrand, totalBkgnd, resultPok, resultPerr, resultPrand, totalBkgnd, key);

    Log(DEBUG, "resultArand = %f", resultArand);
    Log(DEBUG, "resultBok = %f", resultBok);
    Log(DEBUG, "resultBerr = %f", resultBerr);
    Log(DEBUG, "resultBrand = %f", resultBrand);
    Log(DEBUG, "resultBbkgnd = %f", resultBbkgnd);
    Log(DEBUG, "resultCok = %f", resultCok);
    Log(DEBUG, "resultCerr = %f", resultCerr);

    double sifted_time = resultAok/((double) resultAok+resultAerr);
    double sifted_phase = resultBok/((double) resultBok+resultBerr+resultCerr);
    double error_time = resultAerr/((double) resultAok+resultAerr);
    double error_phase = resultCerr/((double) resultBok+resultBerr+resultCerr);
    plot_qkd_stats(sifted_time, sifted_phase, error_time, error_phase, key);

    if (this->phaseStabilizationThread) {
        pthread_mutex_lock(&phaseStabilizationThread->m);
        phaseStabilizationThread->qber_array.push_back(qber());
        phaseStabilizationThread->qber_array.back().start = start;
        phaseStabilizationThread->qber_array.back().stop = stop;
        phaseStabilizationThread->qber_array.back().error = resultCerr/(resultBok + resultCerr);
        phaseStabilizationThread->qber_array.back().std_error = sqrt(resultCerr)/(resultBok + resultCerr);
        pthread_mutex_unlock(&phaseStabilizationThread->m);
    }

    Log(DEBUG, "show_histograms() done");
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
            Log(WARNING, "unknown qubit sequence character");
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
            Log(WARNING, "unknown qubit sequence character");
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
            Log(WARNING, "unknown qubit sequence character");
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
void MainWindow::show_rates(int *channels, double *rates, int n)
{
    int i;

    for (i = 0; i < 18; i++)
        rate_widgets[i]->display(0);

    for (i = 0; i < n; i++)
        rate_widgets[channels[i]-1]->display(rates[i]);

    Log(DEBUG, "freeing rates");

    free(channels);
    free(rates);
}

void MainWindow::parametersChanged(void)
{
    int i;

    for (i = 0; i < 18; i++) {
        if (enabled_mask & (1 << i)) {
            threshold_widgets[i]->setEnabled(true);
            delay_widgets[i]->setEnabled(true);
            test_widgets[i]->setEnabled(true);
            rof_widgets[i]->setEnabled(true);
        } else {
            threshold_widgets[i]->setEnabled(false);
            delay_widgets[i]->setEnabled(false);
            test_widgets[i]->setEnabled(false);
            rof_widgets[i]->setEnabled(false);
        }
        thresholds[i] = threshold_widgets[i]->value();
        delay[i] = delay_widgets[i]->value();
        test[i] = test_widgets[i]->currentIndex();
        rof[i] = rof_widgets[i]->currentIndex();
    }

    if (!s.t) {
        Log(NOTICE, "no time tagger connected");
        return;
    }

    int rising_channel_mask = 0;
    for (i = 0; i < MAX_CHANNELS; i++) {
        if (!(enabled_mask & (1 << i))) continue;
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

void MainWindow::plot_voltage(double voltage)
{
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    ui->qkd_siftedplot->graph(0)->addData(key-lastPointKey_tab3, voltage);
    //ui->qkd_siftedplot->graph(1)->addData(key-lastPointKey_tab3, sifted_phase);
    ui->qkd_siftedplot->xAxis->setRange(key-lastPointKey_tab1, 120, Qt::AlignRight);
    ui->qkd_siftedplot->rescaleAxes();
    ui->qkd_siftedplot->replot();
}

void MainWindow::plot_qkd_stats(double sifted_time, double sifted_phase, double error_time, double error_phase, double key)
{
    ui->qkd_errorplot->graph(0)->addData(key-lastPointKey_tab3, error_time);
    ui->qkd_errorplot->graph(1)->addData(key-lastPointKey_tab3, error_phase);

    //ui->qkd_siftedplot->graph(0)->addData(key-lastPointKey_tab3, sifted_time);
    //ui->qkd_siftedplot->graph(1)->addData(key-lastPointKey_tab3, sifted_phase);

    ui->qkd_errorplot->xAxis->setRange(key-lastPointKey_tab1, 120, Qt::AlignRight);
    //ui->qkd_siftedplot->xAxis->setRange(key-lastPointKey_tab1, 120, Qt::AlignRight);

    ui->qkd_errorplot->rescaleAxes();
    //ui->qkd_siftedplot->rescaleAxes();

    ui->qkd_errorplot->replot();
    //ui->qkd_siftedplot->replot();
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
    int i;
    char key[256];

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
    QMap<QString, QString> mapstring;

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
    mapint.insert("in_ps_port",ui->ps_port->value());

    mapstring.insert("in_ps_ip_address",ui->ps_ip_address->text());
    mapstring.insert("in_qubit_sequence",ui->qubit_sequence->toPlainText());

    for (i = 0; i < 18; i++) {
        sprintf(key,"rof%i",i);
        mapint.insert(key,rof[i]);
        sprintf(key,"test%i",i);
        mapint.insert(key,test[i]);
        sprintf(key,"threshold%i",i);
        mapdouble.insert(key,thresholds[i]);
        sprintf(key,"delay%i",i);
        mapint.insert(key,delay[i]);
    }

    mapdouble.insert("in_adqtime", ui->adqtime->value());

    out << mapint;
    out << mapdouble;
    out << mapstring;
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
    int i;
    char key[256];

    Log(NOTICE, "loading state from %s", fileName.toStdString().c_str());

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
    QMap<QString, QString> mapstringout;
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_4_5);
    in >> mapintout;
    in >> mapdoubleout;
    in >> mapstringout;

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
    if (mapintout.contains("in_ps_port"))
        ui->ps_port->setValue(mapintout.value("in_ps_port"));

    if (mapdoubleout.contains("in_adqtime"))
        ui->adqtime->setValue(mapdoubleout.value("in_adqtime"));

    for (i = 0; i < 18; i++) {
        sprintf(key,"rof%i",i);
        if (mapintout.contains(key)) {
            rof[i] = mapintout.value(key);
            rof_widgets[i]->setCurrentIndex(rof[i]);
        }
        sprintf(key,"test%i",i);
        if (mapintout.contains(key)) {
            test[i] = mapintout.value(key);
            test_widgets[i]->setCurrentIndex(test[i]);
        }
        sprintf(key,"threshold%i",i);
        if (mapdoubleout.contains(key)) {
            thresholds[i] = mapdoubleout.value(key);
            threshold_widgets[i]->setValue(thresholds[i]);
        }
        sprintf(key,"delay%i",i);
        if (mapintout.contains(key)) {
            delay[i] = mapintout.value(key);
            delay_widgets[i]->setValue(delay[i]);
        }
    }

    if (mapstringout.contains("in_ps_ip_address"))
        ui->ps_ip_address->setText(mapstringout.value("in_ps_ip_address"));
    if (mapstringout.contains("in_qubit_sequence"))
        ui->qubit_sequence->setPlainText(mapstringout.value("in_qubit_sequence"));

    QMapIterator<QString,int> iterint(mapintout);
    while (iterint.hasNext()) {
        iterint.next();
        Log(DEBUG, "%s: %i", iterint.key().toStdString().c_str(), iterint.value());
    }

    QMapIterator<QString,double> iterdouble(mapdoubleout);
    while (iterdouble.hasNext()) {
        iterdouble.next();
        Log(DEBUG, "%s: %f", iterdouble.key().toStdString().c_str(), iterdouble.value());
    }

    QMapIterator<QString,QString> iterstring(mapstringout);
    while (iterstring.hasNext()) {
        iterstring.next();
        Log(DEBUG, "%s: %s", iterstring.key().toStdString().c_str(), iterstring.value().toLocal8Bit().data());
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
    Log(NOTICE, "saving last state to default.conf");
    this->SaveState("default.conf");

    Log(NOTICE, "waiting for worker threads to quit");

    if (this->countWorkerThread) {
        this->countWorkerThread->requestInterruption();

        while (this->countWorkerThread->isRunning())
            usleep(100);
    }

    if (this->histogramWorkerThread) {
        this->histogramWorkerThread->requestInterruption();

        while (this->histogramWorkerThread->isRunning())
            usleep(100);
    }

    if (this->phaseStabilizationThread) {
        this->phaseStabilizationThread->requestInterruption();

        while (this->phaseStabilizationThread->isRunning())
            usleep(100);
    }

    while (dbc.isRunning())
        usleep(100);

    Log(NOTICE, "done waiting");

    usleep(1000);

    event->accept();
}
