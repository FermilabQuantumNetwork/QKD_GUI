#ifndef DBCONTROL_H
#define DBCONTROL_H

#include <QString>
#include <QtSql>
#include <QObject>
#include <QtCore>
#include "H5Cpp.h"
#include "qcustomplot.h"
#include "qkd_param.h"
#include "ui_qkd_param.h"


typedef QVector<QVector<bool>> boolvector2d;
typedef QVector<QVector<int>> intvector2d;
typedef QVector<int> intvector;

class DBControl : public QThread
{
    Q_OBJECT

public:
    void run();
    explicit DBControl();
    ~DBControl();
    void disconnectFromServer();
    void DBConnect(QString server, int port, QString database, QString login, QString password);
    H5::H5File *fileh5;
    H5::DataSet *dataset;
    H5::DataSpace *mspace1;
    unsigned long long chunkcounter = 0;
    unsigned long long size[2]={0,0};

    void tryMakeGroupHDF5(QString group_path);

private:
    QSqlDatabase db;
    QSqlDatabase dbqkd;
    bool connection_succesfull;
    bool connectToServerMySQL(QString server, int port, QString database, QString login, QString password);
    const int RANK = 2;

    QString fileh5_name="";

signals:
    void qubitsfromDB(const boolvector2d &qubitsvector, int a, int b);
    void MYtables(QStringList names);

public slots:
    void SaveAndValues(int and1, int and2, int and3, int orgate, int bsm1, int bsm2, float andTime, int delayline);
    void SaveRateValues( int Ra1, int Ra2, int Ra3, int Rb1, int Rb2, int Rb3, int Rc1, int Rc2, int Rc3, float hist_adqtime);

    void readQubits(QString table);
    void SaveQKDresults(double okA,double errA, double randA, double bkgndA,double okB, double errB, double randB,double bkgndB, double okC, double errC, double randC, double bkgndC);
    void SaveQKDstats(int sifted_time, int sifted_phase, double error_rate_time, double error_rate_phase);

    void createHDF5forQKDdata(QString file_name);
    void appendQKDdata2HDF5(const intvector dataokA,const intvector dataerrA, const intvector datarandA, const intvector databkgndA, const intvector dataokB,const intvector dataerrB, const intvector datarandB, const intvector databkgndB, const intvector dataokC,const intvector dataerrC, const intvector datarandC, const intvector databkgndC);

    void savePlotToHDF5(QCustomPlot *plot, QString plot_name, QString group_path);
    void writeAttrToHDF5(QKD_param *param, char qubit_sequence[100000], float adq_time);
};
#endif // DBCONTROL_H
