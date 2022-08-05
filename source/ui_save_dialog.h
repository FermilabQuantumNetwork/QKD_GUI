/********************************************************************************
** Form generated from reading UI file 'save_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SAVE_DIALOG_H
#define UI_SAVE_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Save_dialog
{
public:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QCheckBox *checkBox_det;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QCheckBox *checkBox_h1;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_2;
    QCheckBox *checkBox_h2;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer_3;
    QCheckBox *checkBox_h3;
    QWidget *verticalLayoutWidget_3;
    QVBoxLayout *verticalLayout_3;
    QCheckBox *checkBox_stats;
    QHBoxLayout *horizontalLayout_9;
    QSpacerItem *horizontalSpacer_7;
    QCheckBox *checkBox_stat_time;
    QHBoxLayout *horizontalLayout_10;
    QSpacerItem *horizontalSpacer_8;
    QCheckBox *checkBox_stat_phase;
    QHBoxLayout *horizontalLayout_11;
    QSpacerItem *horizontalSpacer_9;
    QCheckBox *checkBox_stat_voltage;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *checkBox_qb;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer_4;
    QCheckBox *checkBox_early;
    QHBoxLayout *horizontalLayout_7;
    QSpacerItem *horizontalSpacer_5;
    QCheckBox *checkBox_late;
    QHBoxLayout *horizontalLayout_8;
    QSpacerItem *horizontalSpacer_6;
    QCheckBox *checkBox_phase;
    QDialogButtonBox *buttonBox;
    QPushButton *pushButton_file;

    void setupUi(QDialog *Save_dialog)
    {
        if (Save_dialog->objectName().isEmpty())
            Save_dialog->setObjectName(QStringLiteral("Save_dialog"));
        Save_dialog->resize(433, 183);
        verticalLayoutWidget = new QWidget(Save_dialog);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 10, 111, 111));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        checkBox_det = new QCheckBox(verticalLayoutWidget);
        checkBox_det->setObjectName(QStringLiteral("checkBox_det"));

        verticalLayout->addWidget(checkBox_det);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        checkBox_h1 = new QCheckBox(verticalLayoutWidget);
        checkBox_h1->setObjectName(QStringLiteral("checkBox_h1"));

        horizontalLayout_2->addWidget(checkBox_h1);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);

        checkBox_h2 = new QCheckBox(verticalLayoutWidget);
        checkBox_h2->setObjectName(QStringLiteral("checkBox_h2"));

        horizontalLayout_4->addWidget(checkBox_h2);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_3);

        checkBox_h3 = new QCheckBox(verticalLayoutWidget);
        checkBox_h3->setObjectName(QStringLiteral("checkBox_h3"));

        horizontalLayout_5->addWidget(checkBox_h3);


        verticalLayout->addLayout(horizontalLayout_5);

        verticalLayoutWidget_3 = new QWidget(Save_dialog);
        verticalLayoutWidget_3->setObjectName(QStringLiteral("verticalLayoutWidget_3"));
        verticalLayoutWidget_3->setGeometry(QRect(270, 10, 151, 111));
        verticalLayout_3 = new QVBoxLayout(verticalLayoutWidget_3);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        checkBox_stats = new QCheckBox(verticalLayoutWidget_3);
        checkBox_stats->setObjectName(QStringLiteral("checkBox_stats"));

        verticalLayout_3->addWidget(checkBox_stats);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_7);

        checkBox_stat_time = new QCheckBox(verticalLayoutWidget_3);
        checkBox_stat_time->setObjectName(QStringLiteral("checkBox_stat_time"));

        horizontalLayout_9->addWidget(checkBox_stat_time);


        verticalLayout_3->addLayout(horizontalLayout_9);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_8);

        checkBox_stat_phase = new QCheckBox(verticalLayoutWidget_3);
        checkBox_stat_phase->setObjectName(QStringLiteral("checkBox_stat_phase"));

        horizontalLayout_10->addWidget(checkBox_stat_phase);


        verticalLayout_3->addLayout(horizontalLayout_10);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);

        horizontalLayout_11->addItem(horizontalSpacer_9);

        checkBox_stat_voltage = new QCheckBox(verticalLayoutWidget_3);
        checkBox_stat_voltage->setObjectName(QStringLiteral("checkBox_stat_voltage"));

        horizontalLayout_11->addWidget(checkBox_stat_voltage);


        verticalLayout_3->addLayout(horizontalLayout_11);

        verticalLayoutWidget_2 = new QWidget(Save_dialog);
        verticalLayoutWidget_2->setObjectName(QStringLiteral("verticalLayoutWidget_2"));
        verticalLayoutWidget_2->setGeometry(QRect(140, 10, 111, 111));
        verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget_2);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        checkBox_qb = new QCheckBox(verticalLayoutWidget_2);
        checkBox_qb->setObjectName(QStringLiteral("checkBox_qb"));

        verticalLayout_2->addWidget(checkBox_qb);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_4);

        checkBox_early = new QCheckBox(verticalLayoutWidget_2);
        checkBox_early->setObjectName(QStringLiteral("checkBox_early"));

        horizontalLayout_6->addWidget(checkBox_early);


        verticalLayout_2->addLayout(horizontalLayout_6);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_5);

        checkBox_late = new QCheckBox(verticalLayoutWidget_2);
        checkBox_late->setObjectName(QStringLiteral("checkBox_late"));

        horizontalLayout_7->addWidget(checkBox_late);


        verticalLayout_2->addLayout(horizontalLayout_7);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_6);

        checkBox_phase = new QCheckBox(verticalLayoutWidget_2);
        checkBox_phase->setObjectName(QStringLiteral("checkBox_phase"));

        horizontalLayout_8->addWidget(checkBox_phase);


        verticalLayout_2->addLayout(horizontalLayout_8);

        buttonBox = new QDialogButtonBox(Save_dialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(140, 140, 171, 31));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Save);
        buttonBox->setCenterButtons(true);
        pushButton_file = new QPushButton(Save_dialog);
        pushButton_file->setObjectName(QStringLiteral("pushButton_file"));
        pushButton_file->setGeometry(QRect(10, 140, 80, 31));

        retranslateUi(Save_dialog);

        QMetaObject::connectSlotsByName(Save_dialog);
    } // setupUi

    void retranslateUi(QDialog *Save_dialog)
    {
        Save_dialog->setWindowTitle(QApplication::translate("Save_dialog", "Save Plots", Q_NULLPTR));
        checkBox_det->setText(QApplication::translate("Save_dialog", "QKD Det", Q_NULLPTR));
        checkBox_h1->setText(QApplication::translate("Save_dialog", "1", Q_NULLPTR));
        checkBox_h2->setText(QApplication::translate("Save_dialog", "2", Q_NULLPTR));
        checkBox_h3->setText(QApplication::translate("Save_dialog", "3", Q_NULLPTR));
        checkBox_stats->setText(QApplication::translate("Save_dialog", "QKD Stats", Q_NULLPTR));
        checkBox_stat_time->setText(QApplication::translate("Save_dialog", "Error Time", Q_NULLPTR));
        checkBox_stat_phase->setText(QApplication::translate("Save_dialog", "Error Phase", Q_NULLPTR));
        checkBox_stat_voltage->setText(QApplication::translate("Save_dialog", "Voltage", Q_NULLPTR));
        checkBox_qb->setText(QApplication::translate("Save_dialog", "QKD QB", Q_NULLPTR));
        checkBox_early->setText(QApplication::translate("Save_dialog", "Early", Q_NULLPTR));
        checkBox_late->setText(QApplication::translate("Save_dialog", "Late", Q_NULLPTR));
        checkBox_phase->setText(QApplication::translate("Save_dialog", "Phase", Q_NULLPTR));
        pushButton_file->setText(QApplication::translate("Save_dialog", "File Name", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Save_dialog: public Ui_Save_dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SAVE_DIALOG_H
