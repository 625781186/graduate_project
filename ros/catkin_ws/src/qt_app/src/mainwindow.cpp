﻿#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "std_msgs/String.h"
#include <signal.h>
#include <iostream>
#include <QtGui>
#include <QMessageBox>
#include <QListWidgetItem>
#include <geometry_msgs/Twist.h>
#include <iostream>
#include <QSerialPort>
#include <QtWidgets/QVBoxLayout>
#include <QString>
#include "singleton.h"
#include <QDebug>
#include <QMessageBox>
#include "settingparameters.h"
#include "calculation.h"
#include <QMetaType>

using namespace std;

MainWindow::MainWindow(int argc, char **argv, QWidget *parent) : QMainWindow(parent),
                                                                 ui(new Ui::MainWindow),
                                                                 argc1(argc),
                                                                 argv1(argv),
                                                                 m_settings(new SettingsDialog),
                                                                 m_status(new QLabel),
                                                                 m_timer(new QTimer(this)),
                                                                 m_degree(0.0),
                                                                 m_angle(0.0),
                                                                 thread(new QThread(this)),
                                                                 serial(new serialportThread()),
                                                                 period(new QTimer(this))
{
    ui->setupUi(this);
    ui->accelerationEdit->setText(QString::number(ui->accelerationSliderBar->value()));
    ui->statusBar->addWidget(m_status);
    connect(ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::show);
    connect(ui->actionClear, &QAction::triggered, this, &MainWindow::clearLog);
//    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readMyCom);
//    connect(m_serial_2, &QSerialPort::readyRead, this, &MainWindow::readMyCom_2);

    //关联串口线程槽函数
    //打开串口
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    qRegisterMetaType<Settings>("Settings");
    connect(this, SIGNAL(signalOpenSerialPort(Settings, Settings)), serial, SLOT(openSerialPort(Settings, Settings)));
    //打开串口成功
    connect(serial, SIGNAL(openSerialPortSuccess()), this, SLOT(openSerialPortSuccess()));
    //打开串口失败
    connect(serial, SIGNAL(openSerialPortFail(QString)), this, SLOT(openSerialPortFail(QString)));
    //关闭串口
    connect(ui->actionDisconnect, &QAction::triggered, serial, &serialportThread::closeSerialPort);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    //摇杆释放
    connect(ui->rocker, SIGNAL(signalButtonReleased()), serial, SLOT(customButtonReleased()));
    //摇杆按住并移动
    connect(this, SIGNAL(signalButtonMoved(double, double)), serial, SLOT(customButtonMoved(double, double)));
    //定时器定时发送控制信号并轮询轮毂电机位置
    connect(period, &QTimer::timeout, serial, &serialportThread::periodReadWrite);
    connect(period, &QTimer::timeout, this, &MainWindow::restartTimer);
    //修改加速度
    connect(this, SIGNAL(signalChangeAcceleration(int)), serial, SLOT(changeAcceleration(int)));

    connect(serial, SIGNAL(arduinoReceived(QByteArray)), this, SLOT(showArduinoContent(QByteArray)));
    //串口发送命令全部移入新线程执行
    serial->moveToThread(thread);
    thread->start();

}

MainWindow::~MainWindow()
{
    delete m_settings;
    delete m_status;
    delete m_timer;
    delete ui;
}

void MainWindow::restartTimer()
{
    period->start(100);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    delete m_settings;
    delete m_status;
    delete m_timer;
    delete ui;
}

void MainWindow::openSerialPort()
{
    const Settings p = m_settings->settings();
    const Settings p_2 = m_settings->settings_2();
    emit signalOpenSerialPort(p, p_2);

}

void MainWindow::openSerialPortSuccess()
{
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
    ui->actionConfigure->setEnabled(false);
    Settings p = m_settings->settings();
    showStatusMessage(tr("Connected to DRIVERS : %1, %2, %3, %4, %5")
                          .arg(p.stringBaudRate)
                          .arg(p.stringDataBits)
                          .arg(p.stringParity)
                          .arg(p.stringStopBits)
                          .arg(p.stringFlowControl));
    p = m_settings->settings_2();
    showStatusMessage(tr("%1   ||   Connected to ARDUINO : %2, %3, %4, %5, %6")
                          .arg(m_status->text())
                          .arg(p.stringBaudRate)
                          .arg(p.stringDataBits)
                          .arg(p.stringParity)
                          .arg(p.stringStopBits)
                          .arg(p.stringFlowControl));
    period->start(1000);
}

void MainWindow::openSerialPortFail(QString str)
{
    QMessageBox::critical(this, tr("Error"), str);
    showStatusMessage(tr("Open error"));
}

void MainWindow::closeSerialPort()
{
    period->stop();
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    showStatusMessage(tr("Disconnected"));
}

void MainWindow::clearLog()
{
    ui->logWidget->clear();
    ui->logWidget_2->clear();
}

void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}

void chatterCallback(const std_msgs::String::ConstPtr &msg)
{
    ROS_INFO("I heard: [%s]", msg->data.c_str());
}

void MainWindow::on_pushButton_clicked(bool checked)
{
    if (checked)
    {
        ros::init(argc1, argv1, "test_gui");
        if (!ros::master::check())
        {
            return;
        }
        ros::start(); // explicitly needed since our nodehandle is going out of scope.
        ros::NodeHandle n;
        ros::NodeHandle nSub;
        // Add your ros communications here.
        chatter_publisher = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 1000);
        //        chatter_subscriber = nSub.subscribe("testgui_chat", 100, chatterCallback);
        return;
    }
    else
        ros::shutdown();
}

void MainWindow::on_pushButton_2_pressed()
{
    geometry_msgs::Twist msg;
    msg.linear.x = 2.0;
    msg.linear.y = 0.0;
    msg.linear.z = 0.0;
    msg.angular.x = 0.0;
    msg.angular.y = 0.0;
    msg.angular.z = 1.8;
    chatter_publisher.publish(msg);

    return;
}

void MainWindow::showMy485Recv(QString str)
{
    QListWidgetItem *listItem;
    if(str.contains("FAIL"))
    {
        QString res = "ERROR:\t" + str;
        listItem = new QListWidgetItem(res);
        listItem->setBackground(Qt::red);
    }
    else
    {
        QString res = "INFO:\t" + str;
        listItem = new QListWidgetItem(res);
    }
    ui->logWidget->addItem(listItem);
    ui->logWidget->setCurrentRow(ui->logWidget->count() - 1);
}

void MainWindow::showArduinoContent(QByteArray content) //
{
    QListWidgetItem *listItem = new QListWidgetItem(content);
    listItem->setBackground(Qt::green);
    ui->logWidget_2->addItem(listItem);
    ui->logWidget_2->setCurrentRow(ui->logWidget_2->count() - 1);
}

void MainWindow::on_accelerationSliderBar_valueChanged(int value)
{
    ui->accelerationEdit->setText(QString::number(value));
}

void MainWindow::on_applyButton_clicked()
{
    emit signalChangeAcceleration(ui->accelerationSliderBar->value());
}

void MainWindow::on_rocker_signalButtonMoved(int degree, int angle)
{
    Q_UNUSED(degree)
    if (angle < 180)
    {
        m_degree = -ui->velSpinBox->value() * 8192 / 3000;
        angle += 180;
    }
    else
    {
        m_degree = ui->velSpinBox->value() * 8192 / 3000;
    }
    m_angle = 1.0 * (270 - angle) / 90 * ui->angleSpinBox->value();

    emit signalButtonMoved(m_degree, m_angle);
}
