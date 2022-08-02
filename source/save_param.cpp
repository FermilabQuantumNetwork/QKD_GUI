#include "save_param.h"
#include "ui_save_param.h"

Save_param::Save_param(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Save_param)
{
    ui->setupUi(this);
}

Save_param::~Save_param()
{
    delete ui;
}
