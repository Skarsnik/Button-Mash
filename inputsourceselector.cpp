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
    QDialog(parent),
    ui(new Ui::InputSourceSelector)
{
    ui->setupUi(this);
    QLoggingCategory::setFilterRules(QStringLiteral("qt.gamepad.debug=true"));
    usb2snes = nullptr;
    snesClassicTelnet = nullptr;
    arduinoCom = nullptr;
    connect(ui->sourceRadioGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onSourceButtonClicked(QAbstractButton *)));
}

InputSourceSelector::~InputSourceSelector()
{
    delete ui;
}

InputProvider *InputSourceSelector::currentProvider()
{
    return m_currentProvider;
}

InputProvider *InputSourceSelector::getLastProvider(QSettings* settings)
{
    /*if (!settings->contains("inputSource"))
        return nullptr;*/
    //TODO
    snesClassicTelnet = new SNESClassicTelnet();
    ui->snesClassicRadioButton->setChecked(true);
    //arduinoCom = new ArduinoCOM("COM6");
    //return arduinoCom;
    return snesClassicTelnet;
}

void InputSourceSelector::scanDevices()
{
    if (usb2snes == nullptr)
        usb2snes = new USB2snes(false);
    QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    foreach (QSerialPortInfo pfi, infos)
    {
        qDebug() << pfi.description() << pfi.portName();
    }

    if (!infos.isEmpty())
        setArduinoInfo();
    auto gamepads = QGamepadManager::instance()->connectedGamepads();
    qDebug() << "Gamepads : " << gamepads.size();
    /*if (!gamepads.isEmpty())
        setXInputDevices();*/
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
    ui->arduiType1Radio->setEnabled(true);
    ui->arduiType2Radio->setEnabled(true);
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

void InputSourceSelector::on_buttonBox_accepted()
{
    accept();
}

void InputSourceSelector::on_buttonBox_rejected()
{
    reject();
}


int InputSourceSelector::exec()
{
    scanDevices();
    return QDialog::exec();
}

void InputSourceSelector::on_refreshButton_clicked()
{
    scanDevices();
}

void InputSourceSelector::onSourceButtonClicked(QAbstractButton *but)
{
    if (but == ui->snesClassicRadioButton)
    {
        if (snesClassicTelnet == nullptr)
            snesClassicTelnet = new SNESClassicTelnet();
        m_currentProvider = snesClassicTelnet;
    }
    if (but == ui->arduinoRadioButton)
    {
        if (arduinoCom == nullptr)
            arduinoCom = new ArduinoCOM(ui->arduinoComComboBox->currentText());
        m_currentProvider = arduinoCom;
    }
}

void InputSourceSelector::on_arduinoComComboBox_currentIndexChanged(const QString &arg1)
{
    if (arduinoCom != nullptr)
        arduinoCom->setPort(arg1);
}
