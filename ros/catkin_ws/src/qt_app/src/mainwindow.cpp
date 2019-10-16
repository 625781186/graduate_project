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

using namespace std;

MainWindow::MainWindow(int argc, char **argv, QWidget *parent) : QMainWindow(parent),
                                                                 ui(new Ui::MainWindow),
                                                                 argc1(argc),
                                                                 argv1(argv),
                                                                 m_settings(new SettingsDialog),
                                                                 m_serial(new QSerialPort(this)),
                                                                 m_serial_2(new QSerialPort(this)),
                                                                 m_status(new QLabel),
                                                                 m_timer(new QTimer(this)),
                                                                 m_degree(0),
                                                                 m_angle(90)
{
    ui->setupUi(this);
    ui->rpmSliderBar->setValue(100);
    ui->rpmEdit->setText(QString::number(ui->rpmSliderBar->value()));
    ui->accelerationSliderBar->setValue(5);
    ui->accelerationEdit->setText(QString::number(ui->accelerationSliderBar->value()));
    ui->statusBar->addWidget(m_status);
    connect(ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::show);
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(ui->actionClear, &QAction::triggered, this, &MainWindow::clearLog);
//    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readMyCom);
    connect(m_serial_2, &QSerialPort::readyRead, this, &MainWindow::readMyCom_2);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::on_rocker_signalButtonClicked);

}

MainWindow::~MainWindow()
{
    delete m_settings;
    delete m_serial;
    delete m_serial_2;
    delete m_status;
    delete m_timer;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    delete m_settings;
    if (m_serial->isOpen())
        m_serial->close();
    if (m_serial_2->isOpen())
        m_serial_2->close();
    delete m_serial;
    delete m_serial_2;
    delete m_status;
    delete m_timer;
    delete ui;
}

void MainWindow::openSerialPort()
{
    // 打开485串口
    const Settings p = m_settings->settings();
    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.dataBits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);
    if (m_serial->open(QIODevice::ReadWrite))
    {
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->actionConfigure->setEnabled(false);
        showStatusMessage(tr("Connected to DRIVERS : %1, %2, %3, %4, %5")
                              .arg(p.stringBaudRate)
                              .arg(p.stringDataBits)
                              .arg(p.stringParity)
                              .arg(p.stringStopBits)
                              .arg(p.stringFlowControl));
        showMy485Recv(Singleton<command>::GetInstance()->powerOn(m_serial));
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), m_serial->errorString());
        showStatusMessage(tr("Open error"));
    }
    // 打开arduino通信串口
    const Settings p_2 = m_settings->settings_2();
    m_serial_2->setPortName(p_2.name);
    m_serial_2->setBaudRate(p_2.baudRate);
    m_serial_2->setDataBits(p_2.dataBits);
    m_serial_2->setParity(p_2.parity);
    m_serial_2->setStopBits(p_2.stopBits);
    m_serial_2->setFlowControl(p_2.flowControl);
    if (m_serial_2->open(QIODevice::ReadWrite))
    {
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->actionConfigure->setEnabled(false);
        showStatusMessage(tr("%1   ||   Connected to ARDUINO : %2, %3, %4, %5, %6")
                              .arg(m_status->text())
                              .arg(p_2.stringBaudRate)
                              .arg(p_2.stringDataBits)
                              .arg(p_2.stringParity)
                              .arg(p_2.stringStopBits)
                              .arg(p_2.stringFlowControl));
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), m_serial_2->errorString());
        showStatusMessage(tr("Open error"));
    }
}

void MainWindow::closeSerialPort()
{
    if (m_serial->isOpen())
        m_serial->close();
    if (m_serial_2->isOpen())
        m_serial_2->close();
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

void MainWindow::on_sendButton_clicked()
{
    QString str = ui->sendEdit->text(); //��LineEdit�õ��ַ���
    str += Singleton<crc>::GetInstance()->getCrc16(str);
    QByteArray sendData = QByteArray::fromHex(str.toLocal8Bit());
    m_serial->write(sendData); //���͵�����
    str.clear();
    str = "Send:\t" + sendData.toHex().toUpper();
    QListWidgetItem *listItem = new QListWidgetItem(str);
    ui->logWidget->addItem(listItem);
    ui->logWidget->setCurrentRow(ui->logWidget->count() - 1);
}

void MainWindow::on_sendButton_2_clicked()
{
    QString str = ui->sendEdit_2->text();
    QByteArray sendData = str.toLatin1();
    if (sendData.size() == 1)
        sendData.insert(0, "00");
    else if (sendData.size() == 2)
        sendData.insert(0, '0');
    m_serial_2->write(sendData); //���͵�����
    str.clear();
    str = "Send:\t" + sendData.toUpper();
    QListWidgetItem *listItem = new QListWidgetItem(str);
    ui->logWidget_2->addItem(listItem);
    ui->logWidget_2->setCurrentRow(ui->logWidget_2->count() - 1);
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

void MainWindow::readMyCom_2() //��ȡ���������
{
    if (m_serial_2->bytesAvailable() >= 3)
    {
        QByteArray receiveData = m_serial_2->read(3);
        QString str = "Receive:\t" + receiveData.toUpper();
        QListWidgetItem *listItem = new QListWidgetItem(str);
        listItem->setBackground(Qt::green);
        ui->logWidget_2->addItem(listItem);
        ui->logWidget_2->setCurrentRow(ui->logWidget_2->count() - 1);
    }
}

void MainWindow::on_goFront_pressed()
{
    int rpm = ui->rpmSliderBar->value() * 8192 / 3000;
    Singleton<command>::GetInstance()->ctlRpm(m_serial, 1, rpm);
}

void MainWindow::on_rpmSliderBar_valueChanged(int value)
{
    ui->rpmEdit->setText(QString::number(value));
    //    if(m_angle < 180)
    //        m_degree = - value * 8192 / 3000;
    //    else
    //        m_degree = value * 8192 / 3000;
}

void MainWindow::on_goBack_pressed()
{
    int rpm = (-1) * ui->rpmSliderBar->value() * 8192 / 3000;
    Singleton<command>::GetInstance()->ctlRpm(m_serial, 1, rpm);
}

void MainWindow::on_accelerationSliderBar_valueChanged(int value)
{
    ui->accelerationEdit->setText(QString::number(value));
}

void MainWindow::on_applyButton_clicked()
{
    if (!m_serial->isOpen())
    {
        QMessageBox::critical(this, tr("错误"), tr("串口未打开"));
    }
    int acc = ui->accelerationSliderBar->value();
    showMy485Recv(Singleton<command>::GetInstance()->ctlAcc(m_serial, acc));
}

void MainWindow::on_rocker_signalButtonMoved(int degree, int angle)
{
    Q_UNUSED(degree)
    qDebug() << angle;
    if (angle < 180)
    {
        m_degree = -ui->rpmSliderBar->value() * 8192 / 3000;
        m_angle = (angle + 60) * 180 / 300;
    }
    else
    {
        m_degree = ui->rpmSliderBar->value() * 8192 / 3000;
        m_angle = (angle - 180 + 60) * 180 / 300;
    }
}

void MainWindow::on_rocker_signalButtonReleased()
{
    m_timer->stop();
    //    m_degree = 0;
    m_angle = 90;
    showMy485Recv(Singleton<command>::GetInstance()->ctlRpm(m_serial, 1, 0));
    Singleton<command>::GetInstance()->ctlAngle(m_serial_2, 1, m_angle);
    showMy485Recv(Singleton<command>::GetInstance()->ctlRpm(m_serial, 2, 0));
    Singleton<command>::GetInstance()->ctlAngle(m_serial_2, 2, m_angle);
    //    showMy485Recv(Singleton<command>::GetInstance()->ctlRpm(m_serial, 3, 0));
    //    Singleton<command>::GetInstance()->ctlAngle(m_serial_2, 3, m_angle);
    showMy485Recv(Singleton<command>::GetInstance()->ctlRpm(m_serial, 4, 0));
    Singleton<command>::GetInstance()->ctlAngle(m_serial_2, 4, m_angle);
}

void MainWindow::on_rocker_signalButtonClicked()
{
    showMy485Recv(Singleton<command>::GetInstance()->ctlRpm(m_serial, 2, m_degree));
    Singleton<command>::GetInstance()->ctlAngle(m_serial_2, 2, m_angle);
    showMy485Recv(Singleton<command>::GetInstance()->ctlRpm(m_serial, 1, m_degree));
    Singleton<command>::GetInstance()->ctlAngle(m_serial_2, 1, m_angle);
    //    QTime _Timer = QTime::currentTime().addMSecs(100);
    //    while( QTime::currentTime() < _Timer );

    //    showMy485Recv(Singleton<command>::GetInstance()->ctlRpm(m_serial, 3, m_degree));
    //    Singleton<command>::GetInstance()->ctlAngle(m_serial_2, 3, m_angle);
    showMy485Recv(Singleton<command>::GetInstance()->ctlRpm(m_serial, 4, m_degree));
    Singleton<command>::GetInstance()->ctlAngle(m_serial_2, 4, m_angle);
    m_timer->start(100);
}

void MainWindow::on_goLeftButton_pressed()
{
    QString str = "135";
    QByteArray sendData = str.toLatin1();
    m_serial_2->write(sendData); //
    str.clear();
    str = "Send:\t" + sendData.toUpper();
    QListWidgetItem *listItem = new QListWidgetItem(str);
    ui->logWidget_2->addItem(listItem);
    ui->logWidget_2->setCurrentRow(ui->logWidget_2->count() - 1);
}

void MainWindow::on_goLeftButton_released()
{
    QString str = "90";
    QByteArray sendData = str.toLatin1();
    m_serial_2->write(sendData); //
    str.clear();
    str = "Send:\t" + sendData.toUpper();
    QListWidgetItem *listItem = new QListWidgetItem(str);
    ui->logWidget_2->addItem(listItem);
    ui->logWidget_2->setCurrentRow(ui->logWidget_2->count() - 1);
}
