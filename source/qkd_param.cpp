#include "qkd_param.h"
#include "ui_qkd_param.h"

QKD_param::QKD_param(QWidget *parent) : QWidget(parent), ui(new Ui::QKD_param)
{
    ui->setupUi(this);

    QApplication::connect(ui->QKD_timeA, SIGNAL(valueChanged(double)), this, SLOT(slot_QKD_timeA(double)));
    QApplication::connect(ui->QKD_timeB, SIGNAL(valueChanged(double)), this, SLOT(slot_QKD_timeB(double)));
    QApplication::connect(ui->QKD_timeC, SIGNAL(valueChanged(double)), this, SLOT(slot_QKD_timeC(double)));
    QApplication::connect(ui->QKD_numbA, SIGNAL(valueChanged(int)), this, SLOT(slot_QKD_numbA(int)));
    QApplication::connect(ui->QKD_numbB, SIGNAL(valueChanged(int)), this, SLOT(slot_QKD_numbB(int)));
    QApplication::connect(ui->QKD_numbC, SIGNAL(valueChanged(int)), this, SLOT(slot_QKD_numbC(int)));
    QApplication::connect(ui->QKD_phA, SIGNAL(valueChanged(int)), this, SLOT(slot_QKD_phA(int)));
    QApplication::connect(ui->QKD_phB, SIGNAL(valueChanged(int)), this, SLOT(slot_QKD_phB(int)));
    QApplication::connect(ui->QKD_phC, SIGNAL(valueChanged(int)), this, SLOT(slot_QKD_phC(int)));
    QApplication::connect(ui->QKD_iwA, SIGNAL(valueChanged(int)), this, SLOT(slot_QKD_iwA(int)));
    QApplication::connect(ui->QKD_iwB, SIGNAL(valueChanged(int)), this, SLOT(slot_QKD_iwB(int)));
    QApplication::connect(ui->QKD_iwC, SIGNAL(valueChanged(int)), this, SLOT(slot_QKD_iwC(int)));
    QApplication::connect(ui->QKD_zeroA, SIGNAL(valueChanged(int)), this, SLOT(slot_QKD_zeroA(int)));
    QApplication::connect(ui->QKD_zeroB, SIGNAL(valueChanged(int)), this, SLOT(slot_QKD_zeroB(int)));
    QApplication::connect(ui->QKD_zeroC, SIGNAL(valueChanged(int)), this, SLOT(slot_QKD_zeroC(int)));

    QObject::connect(ui->QKD_DB, SIGNAL(valueChanged(int)), this, SLOT(turnONDB(int)));

    QApplication::connect(ui->hdf5, SIGNAL(released()), this, SLOT(in_savehdf5()));

    this->setWindowFlags(Qt::WindowStaysOnTopHint);
}

void QKD_param::QKD_setDefault()
{
    ui->QKD_timeA->setValue(160);
    ui->QKD_numbA->setValue(10);
    ui->QKD_phA->setValue(80);
    ui->QKD_iwA->setValue(40);
    ui->QKD_zeroA->setValue(0);

    ui->QKD_timeB->setValue(160);
    ui->QKD_numbB->setValue(10);
    ui->QKD_phB->setValue(80);
    ui->QKD_iwB->setValue(40);
    ui->QKD_zeroC->setValue(0);

    ui->QKD_timeC->setValue(160);
    ui->QKD_numbC->setValue(10);
    ui->QKD_phC->setValue(80);
    ui->QKD_iwC->setValue(40);
    ui->QKD_zeroB->setValue(0);
}

QKD_param::~QKD_param()
{
    delete ui;
}
