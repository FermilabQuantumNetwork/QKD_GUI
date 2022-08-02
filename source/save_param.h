#ifndef SAVE_PARAM_H
#define SAVE_PARAM_H

#include <QDialog>

namespace Ui {
class Save_param;
}

class Save_param : public QDialog
{
    Q_OBJECT

public:
    explicit Save_param(QWidget *parent = nullptr);
    ~Save_param();

private:
    Ui::Save_param *ui;
};

#endif // SAVE_PARAM_H
