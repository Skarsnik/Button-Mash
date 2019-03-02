#include "inputsourceselector.h"
#include "ui_inputsourceselector.h"

#include <QSerialPortInfo>
#include <QDebug>
#include <QTcpSocket>
#include <QGamepadManager>
#include <QGamepad>
#include <QLoggingCategory>
#include <dinput.h>

InputSourceSelector::InputSourceSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InputSourceSelector)
{
    ui->setupUi(this);
    QLoggingCategory::setFilterRules(QStringLiteral("qt.gamepad.debug=true"));
    usb2snes = nullptr;
}

InputSourceSelector::~InputSourceSelector()
{
    delete ui;
}

void InputSourceSelector::scanDevices()
{
    if (usb2snes == nullptr)
        usb2snes = new USB2snes(false);
    QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    if (!infos.isEmpty())
        setArduinoInfo();
    auto gamepads = QGamepadManager::instance()->connectedGamepads();
    qDebug() << "Gamepads : " << gamepads.size();
    if (!gamepads.isEmpty())
        setXInputDevices();
    foreach (QSerialPortInfo pInfo, infos)
    {
        qDebug() << pInfo.portName() << pInfo.description() << pInfo.serialNumber() << pInfo.manufacturer();
    }
    QTcpSocket  testSocket;
    testSocket.connectToHost("169.254.13.37", 23);
    if (testSocket.waitForConnected(50))
        activateSnesClassicTelnet();
    testSocket.close();
    testSocket.connectToHost("169.254.13.37", 1042);
    if (testSocket.waitForConnected(50))
        activateSnesClassicStuff();
    testSocket.close();
    testSocket.connectToHost("localhost", 8080);
    if (testSocket.waitForConnected(50))
        activateUsb2SnesStuff();
    testSocket.close();
}

void InputSourceSelector::onUsb2SnesConnected()
{
    auto devices = usb2snes->deviceList();
    foreach(QString name, devices)
    {
        ui->usb2snesComboBox->addItem(name);
    }
}

void InputSourceSelector::activateSnesClassicTelnet()
{
    ui->snesClassicRadioButton->setEnabled(true);
}

void InputSourceSelector::activateSnesClassicStuff()
{
    ui->snesClassicStuffRadioButton->setEnabled(true);
}

void InputSourceSelector::activateUsb2SnesStuff()
{
    usb2snes->connect();
    ui->usb2snesRadioButton->setEnabled(true);
}

void InputSourceSelector::setArduinoInfo()
{
    ui->arduinoComComboBox->clear();
    ui->arduinoComComboBox->setEnabled(true);
    ui->arduinoPortLabel->setEnabled(true);
    ui->arduinoRadioButton->setEnabled(true);
    QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    foreach (QSerialPortInfo info, infos)
    {
        ui->arduinoComComboBox->addItem(info.portName());
    }
}

void InputSourceSelector::setXInputDevices()
{
    ui->xinputComboBox->setEnabled(true);
    ui->xinputRadioButton->setEnabled(true);
    ui->xinputComboBox->clear();
    auto gamepads = QGamepadManager::instance()->connectedGamepads();
    foreach(int id, gamepads)
    {
        QGamepad pad(id);
        qDebug() << id;
        qDebug() << QGamepadManager::instance()->gamepadName(id);
        ui->xinputComboBox->addItem(QString("Placeholder name %1").arg(id));
    }
}
