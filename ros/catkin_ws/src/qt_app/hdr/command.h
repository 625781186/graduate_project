#ifndef COMMAND_H
#define COMMAND_H

#include <QSerialPort>
#include <QString>
#include "crc.h"

class command
{
public:
    command();

    QString ctlRpm(QSerialPort *m_serial, qint8 device, int rpm);       //����ת��
    QString ctlAcc(QSerialPort *m_serial, qint8 device, int acc);       //���Ƽ��ٶ�

};

#endif // COMMAND_H
