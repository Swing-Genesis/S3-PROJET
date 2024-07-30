/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCharts>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QPushButton *pb_mode_man;
    QLabel *label_5;
    QLabel *label_9;
    QLCDNumber *Power_Display;
    QLineEdit *lineEdit_dropPosition;
    QSpinBox *DurationBox;
    QLineEdit *lineEdit_Kp;
    QDoubleSpinBox *PWMBox;
    QLCDNumber *Energy_Display;
    QLabel *label_4;
    QLabel *label_3;
    QFrame *line_3;
    QPushButton *pulseButton;
    QLabel *label_pathCSV;
    QLabel *label_10;
    QLineEdit *lineEdit_Ki;
    QLineEdit *lineEdit_Thresh;
    QLabel *label_7;
    QLineEdit *JsonKey;
    QLineEdit *lineEdit_DesVal;
    QLabel *label_2;
    QLabel *label_15;
    QLineEdit *lineEdit_endPosition;
    QLineEdit *lineEdit_slowSpeed;
    QLabel *label_14;
    QLCDNumber *distance_display;
    QComboBox *comboBoxPort;
    QPushButton *pb_rest;
    QLineEdit *lineEdit_Kd;
    QLabel *label_13;
    QCheckBox *checkBox;
    QChartView *graph;
    QLCDNumber *display_pendulum;
    QLabel *label;
    QTextBrowser *textBrowser;
    QLabel *label_8;
    QLineEdit *lineEdit_fastSpeed;
    QLabel *label_11;
    QLabel *label_12;
    QLabel *label_6;
    QFrame *line;
    QLineEdit *lineEdit_initReversePosition;
    QPushButton *pushButton_Params;
    QLabel *label_16;
    QLabel *label_17;
    QLabel *label_18;
    QLabel *label_19;
    QLabel *label_20;
    QPushButton *pb_mode_auto;
    QPushButton *pb_start_auto;
    QPushButton *pb_stop_auto;
    QPushButton *pb_ElectroAimantON;
    QPushButton *pb_ElectroAimantOFF;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(908, 694);
        MainWindow->setAcceptDrops(false);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pb_mode_man = new QPushButton(centralWidget);
        pb_mode_man->setObjectName(QString::fromUtf8("pb_mode_man"));

        gridLayout->addWidget(pb_mode_man, 50, 8, 1, 1);

        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 4, 0, 1, 1);

        label_9 = new QLabel(centralWidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout->addWidget(label_9, 8, 6, 1, 1);

        Power_Display = new QLCDNumber(centralWidget);
        Power_Display->setObjectName(QString::fromUtf8("Power_Display"));

        gridLayout->addWidget(Power_Display, 32, 6, 1, 1);

        lineEdit_dropPosition = new QLineEdit(centralWidget);
        lineEdit_dropPosition->setObjectName(QString::fromUtf8("lineEdit_dropPosition"));

        gridLayout->addWidget(lineEdit_dropPosition, 15, 7, 1, 1);

        DurationBox = new QSpinBox(centralWidget);
        DurationBox->setObjectName(QString::fromUtf8("DurationBox"));
        DurationBox->setKeyboardTracking(false);
        DurationBox->setMinimum(0);
        DurationBox->setMaximum(5000);
        DurationBox->setSingleStep(25);

        gridLayout->addWidget(DurationBox, 8, 1, 1, 1);

        lineEdit_Kp = new QLineEdit(centralWidget);
        lineEdit_Kp->setObjectName(QString::fromUtf8("lineEdit_Kp"));

        gridLayout->addWidget(lineEdit_Kp, 7, 7, 1, 1);

        PWMBox = new QDoubleSpinBox(centralWidget);
        PWMBox->setObjectName(QString::fromUtf8("PWMBox"));
        PWMBox->setMinimum(-1.000000000000000);
        PWMBox->setMaximum(1.000000000000000);
        PWMBox->setSingleStep(0.100000000000000);

        gridLayout->addWidget(PWMBox, 7, 1, 1, 1);

        Energy_Display = new QLCDNumber(centralWidget);
        Energy_Display->setObjectName(QString::fromUtf8("Energy_Display"));

        gridLayout->addWidget(Energy_Display, 32, 7, 1, 1);

        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(label_4, 4, 3, 1, 1);

        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 18, 0, 1, 1);

        line_3 = new QFrame(centralWidget);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setFrameShape(QFrame::VLine);
        line_3->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line_3, 4, 4, 38, 1);

        pulseButton = new QPushButton(centralWidget);
        pulseButton->setObjectName(QString::fromUtf8("pulseButton"));

        gridLayout->addWidget(pulseButton, 9, 0, 1, 2);

        label_pathCSV = new QLabel(centralWidget);
        label_pathCSV->setObjectName(QString::fromUtf8("label_pathCSV"));
        label_pathCSV->setFrameShape(QFrame::StyledPanel);
        label_pathCSV->setTextFormat(Qt::AutoText);

        gridLayout->addWidget(label_pathCSV, 6, 0, 1, 2);

        label_10 = new QLabel(centralWidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout->addWidget(label_10, 9, 6, 1, 1);

        lineEdit_Ki = new QLineEdit(centralWidget);
        lineEdit_Ki->setObjectName(QString::fromUtf8("lineEdit_Ki"));

        gridLayout->addWidget(lineEdit_Ki, 8, 7, 1, 1);

        lineEdit_Thresh = new QLineEdit(centralWidget);
        lineEdit_Thresh->setObjectName(QString::fromUtf8("lineEdit_Thresh"));

        gridLayout->addWidget(lineEdit_Thresh, 10, 7, 1, 1);

        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout->addWidget(label_7, 6, 6, 1, 1);

        JsonKey = new QLineEdit(centralWidget);
        JsonKey->setObjectName(QString::fromUtf8("JsonKey"));
        JsonKey->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(JsonKey, 18, 1, 1, 1);

        lineEdit_DesVal = new QLineEdit(centralWidget);
        lineEdit_DesVal->setObjectName(QString::fromUtf8("lineEdit_DesVal"));

        gridLayout->addWidget(lineEdit_DesVal, 6, 7, 1, 1);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 8, 0, 1, 1);

        label_15 = new QLabel(centralWidget);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        gridLayout->addWidget(label_15, 34, 6, 1, 1);

        lineEdit_endPosition = new QLineEdit(centralWidget);
        lineEdit_endPosition->setObjectName(QString::fromUtf8("lineEdit_endPosition"));

        gridLayout->addWidget(lineEdit_endPosition, 16, 7, 1, 1);

        lineEdit_slowSpeed = new QLineEdit(centralWidget);
        lineEdit_slowSpeed->setObjectName(QString::fromUtf8("lineEdit_slowSpeed"));

        gridLayout->addWidget(lineEdit_slowSpeed, 12, 7, 1, 1);

        label_14 = new QLabel(centralWidget);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        gridLayout->addWidget(label_14, 31, 8, 1, 1);

        distance_display = new QLCDNumber(centralWidget);
        distance_display->setObjectName(QString::fromUtf8("distance_display"));

        gridLayout->addWidget(distance_display, 32, 8, 1, 1);

        comboBoxPort = new QComboBox(centralWidget);
        comboBoxPort->setObjectName(QString::fromUtf8("comboBoxPort"));

        gridLayout->addWidget(comboBoxPort, 4, 1, 1, 1);

        pb_rest = new QPushButton(centralWidget);
        pb_rest->setObjectName(QString::fromUtf8("pb_rest"));

        gridLayout->addWidget(pb_rest, 51, 8, 1, 1);

        lineEdit_Kd = new QLineEdit(centralWidget);
        lineEdit_Kd->setObjectName(QString::fromUtf8("lineEdit_Kd"));

        gridLayout->addWidget(lineEdit_Kd, 9, 7, 1, 1);

        label_13 = new QLabel(centralWidget);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        gridLayout->addWidget(label_13, 31, 7, 1, 1);

        checkBox = new QCheckBox(centralWidget);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));

        gridLayout->addWidget(checkBox, 5, 0, 1, 2);

        graph = new QChartView(centralWidget);
        graph->setObjectName(QString::fromUtf8("graph"));

        gridLayout->addWidget(graph, 24, 0, 19, 4);

        display_pendulum = new QLCDNumber(centralWidget);
        display_pendulum->setObjectName(QString::fromUtf8("display_pendulum"));

        gridLayout->addWidget(display_pendulum, 35, 6, 1, 1);

        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 7, 0, 1, 1);

        textBrowser = new QTextBrowser(centralWidget);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        QFont font;
        font.setPointSize(9);
        textBrowser->setFont(font);

        gridLayout->addWidget(textBrowser, 5, 3, 14, 1);

        label_8 = new QLabel(centralWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout->addWidget(label_8, 7, 6, 1, 1);

        lineEdit_fastSpeed = new QLineEdit(centralWidget);
        lineEdit_fastSpeed->setObjectName(QString::fromUtf8("lineEdit_fastSpeed"));

        gridLayout->addWidget(lineEdit_fastSpeed, 14, 7, 1, 1);

        label_11 = new QLabel(centralWidget);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        gridLayout->addWidget(label_11, 10, 6, 1, 1);

        label_12 = new QLabel(centralWidget);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        gridLayout->addWidget(label_12, 31, 6, 1, 1);

        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_6, 4, 6, 1, 3);

        line = new QFrame(centralWidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout->addWidget(line, 14, 2, 1, 1);

        lineEdit_initReversePosition = new QLineEdit(centralWidget);
        lineEdit_initReversePosition->setObjectName(QString::fromUtf8("lineEdit_initReversePosition"));

        gridLayout->addWidget(lineEdit_initReversePosition, 17, 7, 1, 1);

        pushButton_Params = new QPushButton(centralWidget);
        pushButton_Params->setObjectName(QString::fromUtf8("pushButton_Params"));

        gridLayout->addWidget(pushButton_Params, 18, 6, 1, 3);

        label_16 = new QLabel(centralWidget);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        gridLayout->addWidget(label_16, 12, 6, 1, 1);

        label_17 = new QLabel(centralWidget);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        gridLayout->addWidget(label_17, 14, 6, 1, 1);

        label_18 = new QLabel(centralWidget);
        label_18->setObjectName(QString::fromUtf8("label_18"));

        gridLayout->addWidget(label_18, 15, 6, 1, 1);

        label_19 = new QLabel(centralWidget);
        label_19->setObjectName(QString::fromUtf8("label_19"));

        gridLayout->addWidget(label_19, 16, 6, 1, 1);

        label_20 = new QLabel(centralWidget);
        label_20->setObjectName(QString::fromUtf8("label_20"));

        gridLayout->addWidget(label_20, 17, 6, 1, 1);

        pb_mode_auto = new QPushButton(centralWidget);
        pb_mode_auto->setObjectName(QString::fromUtf8("pb_mode_auto"));

        gridLayout->addWidget(pb_mode_auto, 39, 6, 1, 2);

        pb_start_auto = new QPushButton(centralWidget);
        pb_start_auto->setObjectName(QString::fromUtf8("pb_start_auto"));

        gridLayout->addWidget(pb_start_auto, 50, 6, 1, 2);

        pb_stop_auto = new QPushButton(centralWidget);
        pb_stop_auto->setObjectName(QString::fromUtf8("pb_stop_auto"));

        gridLayout->addWidget(pb_stop_auto, 51, 6, 1, 2);

        pb_ElectroAimantON = new QPushButton(centralWidget);
        pb_ElectroAimantON->setObjectName(QString::fromUtf8("pb_ElectroAimantON"));

        gridLayout->addWidget(pb_ElectroAimantON, 24, 6, 1, 1);

        pb_ElectroAimantOFF = new QPushButton(centralWidget);
        pb_ElectroAimantOFF->setObjectName(QString::fromUtf8("pb_ElectroAimantOFF"));

        gridLayout->addWidget(pb_ElectroAimantOFF, 30, 6, 1, 1);

        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Interface Identification", nullptr));
        pb_mode_man->setText(QCoreApplication::translate("MainWindow", "MODE MAN", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Port:", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "Valeur Ki", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Messages Json de l'Arduino:", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Donnees brutes:", nullptr));
        pulseButton->setText(QCoreApplication::translate("MainWindow", "Commande de pulse", nullptr));
        label_pathCSV->setText(QString());
        label_10->setText(QCoreApplication::translate("MainWindow", "Valeur Kd", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "PID goal", nullptr));
        JsonKey->setText(QCoreApplication::translate("MainWindow", "potVex", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Duree (ms)", nullptr));
        label_15->setText(QCoreApplication::translate("MainWindow", "Pendulum Position:", nullptr));
        label_14->setText(QCoreApplication::translate("MainWindow", "Travelled Distance:", nullptr));
        pb_rest->setText(QCoreApplication::translate("MainWindow", "REST", nullptr));
        label_13->setText(QCoreApplication::translate("MainWindow", "Energy Display:", nullptr));
        checkBox->setText(QCoreApplication::translate("MainWindow", "Enregistrement des donnees sous:", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Tension [-1,1]", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Valeur Kp", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "Seuil", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "Power Display:", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Json sent", nullptr));
        pushButton_Params->setText(QCoreApplication::translate("MainWindow", "Envoie Parametres", nullptr));
        label_16->setText(QCoreApplication::translate("MainWindow", "Slow Speed", nullptr));
        label_17->setText(QCoreApplication::translate("MainWindow", "Fast Speed", nullptr));
        label_18->setText(QCoreApplication::translate("MainWindow", "Drop Position", nullptr));
        label_19->setText(QCoreApplication::translate("MainWindow", "End Position ", nullptr));
        label_20->setText(QCoreApplication::translate("MainWindow", "Init Reverse Position", nullptr));
        pb_mode_auto->setText(QCoreApplication::translate("MainWindow", "MODE AUTO", nullptr));
        pb_start_auto->setText(QCoreApplication::translate("MainWindow", "START", nullptr));
        pb_stop_auto->setText(QCoreApplication::translate("MainWindow", "STOP", nullptr));
        pb_ElectroAimantON->setText(QCoreApplication::translate("MainWindow", "ElectroAimant ON", nullptr));
        pb_ElectroAimantOFF->setText(QCoreApplication::translate("MainWindow", "ElectroAimant OFF", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
