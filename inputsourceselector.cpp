#include "inputsourceselector.h"
#include "ui_inputsourceselector.h"

#include <QSerialPortInfo>
#include <QDebug>
#include <QTcpSocket>
#include <QGamepadManager>
#include <QGamepad>
#include <QLoggingCategory>
#include <dinput.h>

extern QSettings* globalSetting;

const QString SETTING_INPUTSOURCE = "inputSource/inputSource";
const QString SETTING_SNESCLASSIC_TELNET = "SnesClassicTelnet";
const QString SETTING_SNESCLASSIC_STUFF = "SnesClassicStuff";
const QString SETTING_ARDUINO = "Arduino";
const QString SETTING_ARDUINOCOM = "ArduinoCOM";
const QString SETTING_DIRECT_INPUT = "DirectInput";

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

InputProvider *InputSourceSelector::getLastProvider()
{
    InputProvider* toret = nullptr;
    if (globalSetting->contains(SETTING_INPUTSOURCE))
    {
        QString inputSource = globalSetting->value(SETTING_INPUTSOURCE).toString();
        if (inputSource == SETTING_SNESCLASSIC_TELNET)
        {
            snesClassicTelnet = new SNESClassicTelnet();
            ui->snesClassicRadioButton->setChecked(true);
            m_currentProvider = snesClassicTelnet;
            return snesClassicTelnet;
        }
        if (inputSource == SETTING_ARDUINO)
        {
            arduinoCom = new ArduinoCOM(globalSetting->value("inputSource/" + SETTING_ARDUINOCOM).toString());
            ui->arduinoRadioButton->setChecked(true);
            qDebug() << "Using Arduino" << arduinoCom->port();
            m_currentProvider = arduinoCom;
            return arduinoCom;
        }
    } else {
        snesClassicTelnet = new SNESClassicTelnet();
        ui->snesClassicRadioButton->setChecked(true);
        m_currentProvider = snesClassicTelnet;
        toret = snesClassicTelnet;
    }
    //arduinoCom = new ArduinoCOM("COM6");
    //return arduinoCom;
    return toret;
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
    /*testSocket.connectToHost("169.254.13.37", 1042);
    if (testSocket.waitForConnected(50))
        activateSnesClassicStuff();*/
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
    ui->arduinoComComboBox->blockSignals(true);
    ui->arduinoComComboBox->clear();
    ui->arduinoComComboBox->setEnabled(true);
    ui->arduinoPortLabel->setEnabled(true);
    ui->arduinoRadioButton->setEnabled(true);
    ui->arduiType1Radio->setEnabled(true);
    ui->arduiType1Radio->setChecked(true);
    //ui->arduiType2Radio->setEnabled(true);
    QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    unsigned int i = 0;
    int index_arduino = -1;
    foreach (QSerialPortInfo info, infos)
    {
        if (info.description().indexOf("Arduino") != -1)
        {
            ui->arduinoComComboBox->addItem(info.portName() + " - " + info.description());
            index_arduino = i;
        } else {
            ui->arduinoComComboBox->addItem(info.portName());
        }
        ui->arduinoComComboBox->setItemData(i, info.portName(), Qt::UserRole + 1);
        i++;
    }
    if (arduinoCom != nullptr)
    {
        qDebug() << arduinoCom->port();
        unsigned int idx;
        for (idx = 0; idx < ui->arduinoComComboBox->count(); idx++)
        {
            qDebug() << idx << ui->arduinoComComboBox->itemData(idx, Qt::UserRole + 1).toString();
            if (ui->arduinoComComboBox->itemData(idx, Qt::UserRole + 1).toString() == arduinoCom->port())
                break;
        }
        qDebug() << "Found at : " << idx;
        ui->arduinoComComboBox->setCurrentIndex(idx);
    } else {
        if (index_arduino != -1)
            ui->arduinoComComboBox->setCurrentIndex(index_arduino);
    }
    ui->arduinoComComboBox->blockSignals(false);
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
    if (ui->snesClassicRadioButton->isChecked())
        globalSetting->setValue(SETTING_INPUTSOURCE, SETTING_SNESCLASSIC_TELNET);
    if (ui->snesClassicStuffRadioButton->isChecked())
        globalSetting->setValue(SETTING_INPUTSOURCE, SETTING_SNESCLASSIC_STUFF);
    if (ui->arduinoRadioButton->isChecked())
    {
        globalSetting->setValue(SETTING_INPUTSOURCE, SETTING_ARDUINO);
        globalSetting->setValue("inputSource/" + SETTING_ARDUINOCOM, ui->arduinoComComboBox->currentData(Qt::UserRole + 1).toString());
    }
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
            arduinoCom = new ArduinoCOM(ui->arduinoComComboBox->currentData(Qt::UserRole + 1).toString());
        m_currentProvider = arduinoCom;
    }
}

void InputSourceSelector::on_arduinoComComboBox_currentIndexChanged(const QString &arg1)
{
    if (arduinoCom != nullptr)
        arduinoCom->setPort(ui->arduinoComComboBox->currentData(Qt::UserRole + 1).toString());
}
