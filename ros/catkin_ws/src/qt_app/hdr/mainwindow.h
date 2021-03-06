﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include "ros/ros.h"
#include "settingsdialog.h"
#include <QLabel>
#include <QTimer>
#include <QVector>
#include <serialportthread.h>
#include <QButtonGroup>
#include "qnode.h"


//class QLabel;

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int argc, char *argv[], QWidget *parent = nullptr);
    ~MainWindow();
signals:
    void signalOpenSerialPort(Settings p1, Settings p2);
    void signalCloseSerialPort();
    void signalButtonMoved(double degree, double angle);
    void signalChangeAcceleration(int acc);
    void signalClearFault();
    void signalStopNavigation();

public slots:
    void modeChange(int index);
    void openSerialPort();
    void closeSerialPort();
    void clearLog();
    void on_accelerationSliderBar_valueChanged(int value);
    void on_applyButton_clicked();
    void on_rocker_signalButtonMoved(int degree, int angle);
    void restartTimer();
    void openSerialPortSuccess();
    void openSerialPortFail(QString str);
    void showArduinoContent(QByteArray content);
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    int argc1;
    char **argv1;
    QButtonGroup* radioButtonGroup;
    SettingsDialog *m_settings = nullptr;
    QLabel *m_status = nullptr;
    double m_degree;
    double m_angle;
    QByteArray arduinoResponseData;
    QVector<int> rpmVec;
    QThread *thread;
    serialportThread* serial = nullptr;
    QTimer *period = nullptr;
    QNode *listen_cmd_thread = nullptr;

    virtual void closeEvent(QCloseEvent *event);
    void showStatusMessage(const QString &message);
    QByteArray QString2Hex(QString str);
    char ConvertHexChar(char ch);
    void showMy485Recv(QString str);

};

#endif // MAINWINDOW_H
