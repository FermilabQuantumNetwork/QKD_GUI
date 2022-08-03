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

private:

signals:
    // This signal relays which checkboxes are pressed
    void savePressed(bool h1, bool h2, bool h3, bool early, bool late, bool phase, bool time, bool error, bool voltage);
};

#endif // SAVE_DIALOG_H
