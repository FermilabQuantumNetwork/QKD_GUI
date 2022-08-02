#include "save_dialog.h"
#include "ui_save_dialog.h"

Save_dialog::Save_dialog(QWidget *parent) : QDialog(parent), ui(new Ui::Save_dialog)
{
    ui->setupUi(this);

    emit ui->checkBox_det->toggle();
    emit ui->checkBox_qb->toggle();
    emit ui->checkBox_stats->toggle();

    this->setWindowFlags(Qt::WindowStaysOnTopHint);
}

Save_dialog::~Save_dialog()
{
    delete ui;
}

void Save_dialog::on_checkBox_det_toggled(bool checked)
{
    if (checked == true) {
        ui->checkBox_h1->setChecked(true);
        ui->checkBox_h2->setChecked(true);
        ui->checkBox_h3->setChecked(true);
    } else {
        ui->checkBox_h1->setChecked(false);
        ui->checkBox_h2->setChecked(false);
        ui->checkBox_h3->setChecked(false);
    }
}


void Save_dialog::on_checkBox_qb_toggled(bool checked)
{
    if (checked == true) {
        ui->checkBox_early->setChecked(true);
        ui->checkBox_late->setChecked(true);
        ui->checkBox_phase->setChecked(true);
    } else {
        ui->checkBox_early->setChecked(false);
        ui->checkBox_late->setChecked(false);
        ui->checkBox_phase->setChecked(false);
    }
}


void Save_dialog::on_checkBox_stats_toggled(bool checked)
{
    if (checked == true) {
        ui->checkBox_stat_time->setChecked(true);
        ui->checkBox_stat_phase->setChecked(true);
        ui->checkBox_stat_voltage->setChecked(true);
    } else {
        ui->checkBox_stat_time->setChecked(false);
        ui->checkBox_stat_phase->setChecked(false);
        ui->checkBox_stat_voltage->setChecked(false);
    }
}
