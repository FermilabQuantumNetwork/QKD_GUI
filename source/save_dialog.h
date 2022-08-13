#ifndef SAVE_DIALOG_H
#define SAVE_DIALOG_H

#include <QDialog>

namespace Ui {
    class Save_dialog;
}

class Save_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Save_dialog(QWidget *parent = nullptr);
    ~Save_dialog();
    Ui::Save_dialog *ui;

private slots:
    void on_checkBox_det_toggled(bool checked);
    void on_checkBox_qb_toggled(bool checked);
    void on_checkBox_stats_toggled(bool checked);

    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

    void on_pushButton_file_released();

    void on_checkBox_hists_toggled(bool checked);

    bool fileExists(QString path);
    void warnFileExists(QString path);

private:
    QString file_name = "data";
    QString file_path = "../data/data.h5";

signals:
    // This signal relays which checkboxes are pressed
    void savePressed(QString file_path, bool h_time, bool h_phase_ok, bool h_phase_bad, bool h1, bool h2, bool h3, bool early, bool late, bool phase, bool time, bool error, bool voltage);
    void sig_fileExists(QString file_path);
};

#endif // SAVE_DIALOG_H
