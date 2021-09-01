#ifndef QKD_PARAM_H
#define QKD_PARAM_H

#include <QWidget>

namespace Ui {
    class QKD_param;
}

class QKD_param : public QWidget
{
    Q_OBJECT

public:
    explicit QKD_param(QWidget *parent = nullptr);
    ~QKD_param();
    void QKD_setDefault();
    Ui::QKD_param *ui;

private:
    int  in_QKD_numbA, in_QKD_phA, in_QKD_iwA, in_QKD_pxqA, in_QKD_zeroA;
    int  in_QKD_numbB, in_QKD_phB, in_QKD_iwB, in_QKD_pxqB, in_QKD_zeroB;
    int  in_QKD_numbC, in_QKD_phC, in_QKD_iwC, in_QKD_pxqC, in_QKD_zeroC;
    double in_QKD_timeA, in_QKD_timeB,  in_QKD_timeC;

signals:
    void sig_QKD_timeA(double a);
    void sig_QKD_timeB(double a);
    void sig_QKD_timeC(double a);
    void sig_QKD_numbA(int a);
    void sig_QKD_numbB(int a);
    void sig_QKD_numbC(int a);
    void sig_QKD_phA(int a);
    void sig_QKD_phB(int a);
    void sig_QKD_phC(int a);
    void sig_QKD_iwA(int a);
    void sig_QKD_iwB(int a);
    void sig_QKD_iwC(int a);
    void sig_QKD_pxqA(int a);
    void sig_QKD_pxqB(int a);
    void sig_QKD_pxqC(int a);
    void sig_QKD_zeroA(int a);
    void sig_QKD_zeroB(int a);
    void sig_QKD_zeroC(int a);
    void savehdf5();
    void sig_turnONDB(int a);

public slots:

    void slot_QKD_timeA(double a){emit sig_QKD_timeA(a);}
    void slot_QKD_timeB(double a){emit sig_QKD_timeB(a);}
    void slot_QKD_timeC(double a){emit sig_QKD_timeC(a);}
    void slot_QKD_numbA(int a){emit sig_QKD_numbA(a);}
    void slot_QKD_numbB(int a){emit sig_QKD_numbB(a);}
    void slot_QKD_numbC(int a){emit sig_QKD_numbC(a);}
    void slot_QKD_phA(int a){emit sig_QKD_phA(a);}
    void slot_QKD_phB(int a){emit sig_QKD_phB(a);}
    void slot_QKD_phC(int a){emit sig_QKD_phC(a);}
    void slot_QKD_iwA(int a){emit sig_QKD_iwA(a);}
    void slot_QKD_iwB(int a){emit sig_QKD_iwB(a);}
    void slot_QKD_iwC(int a){emit sig_QKD_iwC(a);}
    void slot_QKD_pxqA(int a){emit sig_QKD_pxqA(a);}
    void slot_QKD_pxqB(int a){emit sig_QKD_pxqB(a);}
    void slot_QKD_pxqC(int a){emit sig_QKD_pxqC(a);}
    void slot_QKD_zeroA(int a){sig_QKD_zeroA(a);}
    void slot_QKD_zeroB(int a){sig_QKD_zeroB(a);}
    void slot_QKD_zeroC(int a){sig_QKD_zeroC(a);}
    void in_savehdf5(){emit savehdf5();}
    void turnONDB(int a){emit sig_turnONDB(a);}
};

#endif // QKD_PARAM_H
