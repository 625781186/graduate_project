#include "ui_settingsdialog.h"
#include "settingsdialog.h"

#include <QIntValidator>
#include <QLineEdit>
#include <QSerialPortInfo>

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SettingsDialog),
    m_intValidator(new QIntValidator(0, 4000000, this))
{
    m_ui->setupUi(this);

    m_ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

    connect(m_ui->applyButton, &QPushButton::clicked, this, &SettingsDialog::apply);
    //轮毂电机串口信号
    connect(m_ui->serialPortInfoListBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::showPortInfo);
    connect(m_ui->baudRateBox,  QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::checkCustomBaudRatePolicy);
    connect(m_ui->serialPortInfoListBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::checkCustomDevicePathPolicy);
    //Arduino串口信号
    connect(m_ui->serialPortInfoListBox_2, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::showPortInfo2);
    connect(m_ui->baudRateBox_2,  QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::checkCustomBaudRatePolicy2);
    connect(m_ui->serialPortInfoListBox_2, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::checkCustomDevicePathPolicy2);

    fillPortsParameters();
    fillPortsInfo();

    updateSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete m_intValidator;
    delete m_ui;
}

Settings SettingsDialog::settings() const
{
    return m_currentSettings1;
}

Settings SettingsDialog::settings_2() const
{
    return m_currentSettings2;
}

void SettingsDialog::showPortInfo(int idx)
{
    if (idx == -1)
        return;
    //显示485通讯串口信息
    const QStringList list = m_ui->serialPortInfoListBox->itemData(idx).toStringList();
    m_ui->descriptionLabel->setText(tr("Description: %1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));
    m_ui->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
    m_ui->serialNumberLabel->setText(tr("Serial number: %1").arg(list.count() > 3 ? list.at(3) : tr(blankString)));
    m_ui->locationLabel->setText(tr("Location: %1").arg(list.count() > 4 ? list.at(4) : tr(blankString)));
    m_ui->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.count() > 5 ? list.at(5) : tr(blankString)));
    m_ui->pidLabel->setText(tr("Product Identifier: %1").arg(list.count() > 6 ? list.at(6) : tr(blankString)));
}
//显示Arduino通讯串口信息
void SettingsDialog::showPortInfo2(int idx)
{
    if (idx == -1)
        return;
    const QStringList list2 = m_ui->serialPortInfoListBox_2->itemData(idx).toStringList();
    m_ui->descriptionLabel_2->setText(tr("Description: %1").arg(list2.count() > 1 ? list2.at(1) : tr(blankString)));
    m_ui->manufacturerLabel_2->setText(tr("Manufacturer: %1").arg(list2.count() > 2 ? list2.at(2) : tr(blankString)));
    m_ui->serialNumberLabel_2->setText(tr("Serial number: %1").arg(list2.count() > 3 ? list2.at(3) : tr(blankString)));
    m_ui->locationLabel_2->setText(tr("Location: %1").arg(list2.count() > 4 ? list2.at(4) : tr(blankString)));
    m_ui->vidLabel_2->setText(tr("Vendor Identifier: %1").arg(list2.count() > 5 ? list2.at(5) : tr(blankString)));
    m_ui->pidLabel_2->setText(tr("Product Identifier: %1").arg(list2.count() > 6 ? list2.at(6) : tr(blankString)));
}

void SettingsDialog::apply()
{
    updateSettings();
    hide();
}

void SettingsDialog::checkCustomBaudRatePolicy(int idx)
{
    const bool isCustomBaudRate = !m_ui->baudRateBox->itemData(idx).isValid();
    m_ui->baudRateBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        m_ui->baudRateBox->clearEditText();
        QLineEdit *edit = m_ui->baudRateBox->lineEdit();
        edit->setValidator(m_intValidator);
    }
}

void SettingsDialog::checkCustomBaudRatePolicy2(int idx)
{
    const bool isCustomBaudRate = !m_ui->baudRateBox_2->itemData(idx).isValid();
    m_ui->baudRateBox_2->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        m_ui->baudRateBox_2->clearEditText();
        QLineEdit *edit = m_ui->baudRateBox_2->lineEdit();
        edit->setValidator(m_intValidator);
    }
}

void SettingsDialog::checkCustomDevicePathPolicy(int idx)
{
    const bool isCustomPath = !m_ui->serialPortInfoListBox->itemData(idx).isValid();
    m_ui->serialPortInfoListBox->setEditable(isCustomPath);
    if (isCustomPath)
        m_ui->serialPortInfoListBox->clearEditText();
}

void SettingsDialog::checkCustomDevicePathPolicy2(int idx)
{
    const bool isCustomPath = !m_ui->serialPortInfoListBox_2->itemData(idx).isValid();
    m_ui->serialPortInfoListBox_2->setEditable(isCustomPath);
    if (isCustomPath)
        m_ui->serialPortInfoListBox_2->clearEditText();
}

void SettingsDialog::fillPortsParameters()
{
    // 485串口设置
    m_ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    m_ui->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    m_ui->baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    m_ui->baudRateBox->addItem(QStringLiteral("57600"), QSerialPort::Baud57600);
    m_ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    m_ui->baudRateBox->addItem(tr("Custom"));
    m_ui->baudRateBox->setCurrentIndex(3);

    m_ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    m_ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    m_ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    m_ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    m_ui->dataBitsBox->setCurrentIndex(3);

    m_ui->parityBox->addItem(tr("None"), QSerialPort::NoParity);
    m_ui->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    m_ui->parityBox->addItem(tr("Odd"), QSerialPort::OddParity);
    m_ui->parityBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    m_ui->parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);

    m_ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    m_ui->stopBitsBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    m_ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    m_ui->flowControlBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    m_ui->flowControlBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    m_ui->flowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
    // Arduino串口设置
    m_ui->baudRateBox_2->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    m_ui->baudRateBox_2->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    m_ui->baudRateBox_2->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    m_ui->baudRateBox_2->addItem(QStringLiteral("57600"), QSerialPort::Baud57600);
    m_ui->baudRateBox_2->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    m_ui->baudRateBox_2->addItem(tr("Custom"));
    m_ui->baudRateBox_2->setCurrentIndex(3);

    m_ui->dataBitsBox_2->addItem(QStringLiteral("5"), QSerialPort::Data5);
    m_ui->dataBitsBox_2->addItem(QStringLiteral("6"), QSerialPort::Data6);
    m_ui->dataBitsBox_2->addItem(QStringLiteral("7"), QSerialPort::Data7);
    m_ui->dataBitsBox_2->addItem(QStringLiteral("8"), QSerialPort::Data8);
    m_ui->dataBitsBox_2->setCurrentIndex(3);

    m_ui->parityBox_2->addItem(tr("None"), QSerialPort::NoParity);
    m_ui->parityBox_2->addItem(tr("Even"), QSerialPort::EvenParity);
    m_ui->parityBox_2->addItem(tr("Odd"), QSerialPort::OddParity);
    m_ui->parityBox_2->addItem(tr("Mark"), QSerialPort::MarkParity);
    m_ui->parityBox_2->addItem(tr("Space"), QSerialPort::SpaceParity);

    m_ui->stopBitsBox_2->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    m_ui->stopBitsBox_2->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    m_ui->stopBitsBox_2->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    m_ui->flowControlBox_2->addItem(tr("None"), QSerialPort::NoFlowControl);
    m_ui->flowControlBox_2->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    m_ui->flowControlBox_2->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
}

void SettingsDialog::fillPortsInfo()
{
    m_ui->serialPortInfoListBox->clear();
    m_ui->serialPortInfoListBox_2->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        if(manufacturer.contains("Silicon", Qt::CaseInsensitive))
            m_485Index = m_ui->serialPortInfoListBox->count();
        else if(manufacturer.contains("Arduino", Qt::CaseInsensitive))
            m_arduinoIndex = m_ui->serialPortInfoListBox->count();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

        m_ui->serialPortInfoListBox->addItem(list.first(), list);
        m_ui->serialPortInfoListBox_2->addItem(list.first(), list);
    }

    m_ui->serialPortInfoListBox->addItem(tr("Custom"));
    m_ui->serialPortInfoListBox_2->addItem(tr("Custom"));
    m_ui->serialPortInfoListBox->setCurrentIndex(m_485Index);
    m_ui->serialPortInfoListBox_2->setCurrentIndex(m_arduinoIndex);
}

void SettingsDialog::updateSettings()
{
    m_currentSettings1.name = m_ui->serialPortInfoListBox->currentText();

    if (m_ui->baudRateBox->currentIndex() == 4) {
        m_currentSettings1.baudRate = m_ui->baudRateBox->currentText().toInt();
    } else {
        m_currentSettings1.baudRate = static_cast<QSerialPort::BaudRate>(
                    m_ui->baudRateBox->itemData(m_ui->baudRateBox->currentIndex()).toInt());
    }
    m_currentSettings1.stringBaudRate = QString::number(m_currentSettings1.baudRate);

    m_currentSettings1.dataBits = static_cast<QSerialPort::DataBits>(
                m_ui->dataBitsBox->itemData(m_ui->dataBitsBox->currentIndex()).toInt());
    m_currentSettings1.stringDataBits = m_ui->dataBitsBox->currentText();

    m_currentSettings1.parity = static_cast<QSerialPort::Parity>(
                m_ui->parityBox->itemData(m_ui->parityBox->currentIndex()).toInt());
    m_currentSettings1.stringParity = m_ui->parityBox->currentText();

    m_currentSettings1.stopBits = static_cast<QSerialPort::StopBits>(
                m_ui->stopBitsBox->itemData(m_ui->stopBitsBox->currentIndex()).toInt());
    m_currentSettings1.stringStopBits = m_ui->stopBitsBox->currentText();

    m_currentSettings1.flowControl = static_cast<QSerialPort::FlowControl>(
                m_ui->flowControlBox->itemData(m_ui->flowControlBox->currentIndex()).toInt());
    m_currentSettings1.stringFlowControl = m_ui->flowControlBox->currentText();

    m_currentSettings2.name = m_ui->serialPortInfoListBox_2->currentText();
    if (m_ui->baudRateBox_2->currentIndex() == 4) {
        m_currentSettings2.baudRate = m_ui->baudRateBox_2->currentText().toInt();
    } else {
        m_currentSettings2.baudRate = static_cast<QSerialPort::BaudRate>(
                    m_ui->baudRateBox_2->itemData(m_ui->baudRateBox_2->currentIndex()).toInt());
    }
    m_currentSettings2.stringBaudRate = QString::number(m_currentSettings2.baudRate);

    m_currentSettings2.dataBits = static_cast<QSerialPort::DataBits>(
                m_ui->dataBitsBox_2->itemData(m_ui->dataBitsBox_2->currentIndex()).toInt());
    m_currentSettings2.stringDataBits = m_ui->dataBitsBox_2->currentText();

    m_currentSettings2.parity = static_cast<QSerialPort::Parity>(
                m_ui->parityBox_2->itemData(m_ui->parityBox_2->currentIndex()).toInt());
    m_currentSettings2.stringParity = m_ui->parityBox_2->currentText();

    m_currentSettings2.stopBits = static_cast<QSerialPort::StopBits>(
                m_ui->stopBitsBox_2->itemData(m_ui->stopBitsBox_2->currentIndex()).toInt());
    m_currentSettings2.stringStopBits = m_ui->stopBitsBox_2->currentText();

    m_currentSettings2.flowControl = static_cast<QSerialPort::FlowControl>(
                m_ui->flowControlBox_2->itemData(m_ui->flowControlBox_2->currentIndex()).toInt());
    m_currentSettings2.stringFlowControl = m_ui->flowControlBox_2->currentText();
}
