#include "inputsourceselector.h"
#include "mapbuttondialog.h"
#include "localcontrollermanager.h"
#include "ui_inputsourceselector.h"

#include <QSerialPortInfo>
#include <QDebug>
#include <QTcpSocket>
#include <QGamepadManager>
#include <QGamepad>
#include <QLoggingCategory>

extern QSettings* globalSetting;

const QString SETTING_INPUTSOURCE = "inputSource/inputSource";
const QString SETTING_DELAI = "delai";

const QString SETTING_SNESCLASSIC_TELNET = "SnesClassicTelnet";
const QString SETTING_SNESCLASSIC_STUFF = "SnesClassicStuff";
const QString SETTING_USB2SNES = "Usb2Snes";
const QString SETTING_USB2SNES_DEVICE =  "Usb2SnesDevice";
const QString SETTING_USB2SNES_GAME =  "Usb2SnesGame";
const QString SETTING_ARDUINO = "Arduino";
const QString SETTING_ARDUINOCOM = "ArduinoCOM";
const QString SETTING_DIRECT_INPUT = "DirectInput";
const QString SETTING_LOCAL_CONTROLLER = "LocalController";
const QString SETTING_LOCALCONTROLLER_MAPPING = "LocalController/mapping";
const QString SETTING_LOCALCONTROLLER_DEVICEID = "LocalControllerDeviceId";

InputSourceSelector::InputSourceSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputSourceSelector)
{
    ui->setupUi(this);
    QLoggingCategory::setFilterRules(QStringLiteral("qt.gamepad.debug=true"));
    usb2snes = nullptr;
    snesClassicTelnet = nullptr;
    arduinoCom = nullptr;
    usb2snesProvider = nullptr;
    m_currentProvider = nullptr;
    localcontrollerProvider = nullptr;
    m_delai = 0;
    if (globalSetting->contains(SETTING_DELAI))
    {
        m_delai = globalSetting->value(SETTING_DELAI).toUInt();
        ui->delaiSpinBox->setValue(m_delai);
    }
    connect(ui->sourceRadioGroup, qOverload<QAbstractButton*>(&QButtonGroup::buttonClicked), this, &InputSourceSelector::onSourceButtonClicked);
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
        }
        if (inputSource == SETTING_ARDUINO)
        {
            arduinoCom = new ArduinoCOM(globalSetting->value("inputSource/" + SETTING_ARDUINOCOM).toString());
            ui->arduinoRadioButton->setChecked(true);
            qDebug() << "Using Arduino" << arduinoCom->port();
            m_currentProvider = arduinoCom;
        }
        if (inputSource == SETTING_USB2SNES)
        {
            usb2snes = new USB2snes(false);
            usb2snesProvider = new Usb2SnesSource(usb2snes);
            usb2snesProvider->loadGamesList();
            usb2snesProvider->setDevice(globalSetting->value("inputSource/" + SETTING_USB2SNES_DEVICE).toString());
            usb2snesProvider->setGame(globalSetting->value("inputSource/" + SETTING_USB2SNES_GAME).toString());
            ui->usb2snesRadioButton->setChecked(true);
            m_currentProvider = usb2snesProvider;
        }
        if (inputSource == SETTING_LOCAL_CONTROLLER)
        {
            localcontrollerProvider = LocalControllerManager::getManager()->createProvider(globalSetting->value("inputSource/" + SETTING_LOCALCONTROLLER_DEVICEID).toString());
            localcontrollerMapping = LocalControllerManager::getManager()->loadMapping(*globalSetting, "inputSource/" + SETTING_LOCALCONTROLLER_MAPPING);
            qDebug() << "Number of key binded :" << localcontrollerMapping.size();
            localcontrollerProvider->setMapping(localcontrollerMapping);
            ui->xinputRadioButton->setChecked(true);
            m_currentProvider = localcontrollerProvider;
        }
        return m_currentProvider;
    } else {
        return nullptr;
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
    auto gamepads = LocalControllerManager::getManager()->listController();
    qDebug() << "Gamepads : " << gamepads.size();
    if (!gamepads.isEmpty())
        setLocalControllers();
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

unsigned int InputSourceSelector::delai() const
{
    return m_delai;
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
    if (usb2snesProvider == nullptr)
    {
        usb2snes = new USB2snes(false);
        usb2snesProvider = new Usb2SnesSource(usb2snes);
    }
    auto gamesList = usb2snesProvider->loadGamesList();
    ui->usb2gameComboBox->clear();
    foreach (QString game, gamesList)
    {
        ui->usb2gameComboBox->addItem(game);
    }
    if (usb2snes->state() == USB2snes::None)
        usb2snes->connect();
    if (usb2snes->state() == USB2snes::Connected || usb2snes->state() == USB2snes::Ready)
        onUsb2SnesConnected();
    connect(usb2snes, &USB2snes::connected, this, &InputSourceSelector::onUsb2SnesConnected, Qt::UniqueConnection);
    ui->usb2snesRadioButton->setEnabled(true);
}

void InputSourceSelector::onUsb2SnesConnected()
{
    auto devices = usb2snes->deviceList();
    ui->usb2snesComboBox->clear();
    ui->usb2snesComboBox->setEnabled(true);
    foreach(QString name, devices)
    {
        ui->usb2snesComboBox->addItem(name);
    }
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

void InputSourceSelector::setLocalControllers()
{
    ui->xinputComboBox->blockSignals(true);
    ui->xinputComboBox->setEnabled(true);
    ui->xinputRadioButton->setEnabled(true);
    ui->xinputComboBox->clear();
    auto gamepads = LocalControllerManager::getManager()->listController();
    localcontrollerList = gamepads;
    unsigned int cpt = 0;
    for (auto& gamepad : gamepads)
    {
        //QGamepad pad(id);
        qDebug() << gamepad.id;
        qDebug() << gamepad.name;
        ui->xinputComboBox->addItem(gamepad.name);
        ui->xinputComboBox->setItemData(cpt, gamepad.id, Qt::UserRole + 1);
        cpt++;
    }
    if (localcontrollerMapping.isEmpty() && globalSetting->contains(SETTING_LOCALCONTROLLER_DEVICEID))
    {
        localcontrollerMapping = LocalControllerManager::getManager()->loadMapping(*globalSetting, "inputSource/" + SETTING_LOCALCONTROLLER_MAPPING);
    }
    if (localcontrollerProvider != nullptr)
    {
        unsigned int idx;
        for (idx = 0; idx < ui->xinputComboBox->count(); idx++)
        {
            qDebug() << localcontrollerProvider->id() << ui->xinputComboBox->itemData(idx, Qt::UserRole + 1).toString();
            if (ui->xinputComboBox->itemData(idx, Qt::UserRole + 1).toString() == localcontrollerProvider->id())
                break;
        }
        ui->xinputComboBox->setCurrentIndex(idx);
        ui->mappingButton->setEnabled(true);
    }
    ui->xinputComboBox->blockSignals(false);
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
    if (ui->usb2snesRadioButton->isChecked())
    {
        globalSetting->setValue(SETTING_INPUTSOURCE, SETTING_USB2SNES);
        globalSetting->setValue("inputSource/" + SETTING_USB2SNES_DEVICE, ui->usb2snesComboBox->currentText());
        globalSetting->setValue("inputSource/" + SETTING_USB2SNES_GAME, ui->usb2gameComboBox->currentText());
    }
    if (ui->xinputRadioButton->isChecked())
    {
        globalSetting->setValue(SETTING_INPUTSOURCE, SETTING_LOCAL_CONTROLLER);
        globalSetting->setValue("inputSource/" + SETTING_LOCALCONTROLLER_DEVICEID, ui->xinputComboBox->currentData(Qt::UserRole + 1).toString());
        LocalControllerManager::getManager()->saveMapping(*globalSetting, "inputSource/" + SETTING_LOCALCONTROLLER_MAPPING, localcontrollerMapping);
        localcontrollerProvider->setMapping(localcontrollerMapping);
    }
    m_delai = ui->delaiSpinBox->value();
    if (m_delai != 0)
        globalSetting->setValue(SETTING_DELAI, m_delai);
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
    if (but == ui->usb2snesRadioButton)
    {
        if (usb2snesProvider == nullptr)
            usb2snesProvider = new Usb2SnesSource(usb2snes);
        m_currentProvider = usb2snesProvider;

    }
    if (but == ui->xinputRadioButton)
    {
        ui->mappingButton->setEnabled(true);
        if (localcontrollerProvider == nullptr)
            localcontrollerProvider = LocalControllerManager::getManager()->createProvider(ui->xinputComboBox->currentData(Qt::UserRole + 1).toString());
        if (localcontrollerMapping.isEmpty())
            localcontrollerMapping = LocalControllerManager::getManager()->loadMapping(*globalSetting, SETTING_LOCALCONTROLLER_MAPPING);
        localcontrollerProvider->setMapping(localcontrollerMapping);
        m_currentProvider = localcontrollerProvider;
    }
}

void InputSourceSelector::on_arduinoComComboBox_currentIndexChanged(const QString &arg1)
{
    if (arduinoCom != nullptr)
        arduinoCom->setPort(ui->arduinoComComboBox->currentData(Qt::UserRole + 1).toString());
}

void InputSourceSelector::on_usb2snesComboBox_currentTextChanged(const QString &arg1)
{
    usb2snesProvider->setDevice(arg1);
}

void InputSourceSelector::on_usb2gameComboBox_currentTextChanged(const QString &arg1)
{
    usb2snesProvider->setGame(arg1);
}

void InputSourceSelector::on_xinputComboBox_currentIndexChanged(int index)
{
    if (localcontrollerProvider != nullptr)
        delete localcontrollerProvider;
    localcontrollerProvider = LocalControllerManager::getManager()->createProvider(ui->xinputComboBox->itemData(index, Qt::UserRole + 1).toString());
}


void InputSourceSelector::on_mappingButton_clicked()
{
    MapButtonDialog mapDiag;

    mapDiag.setDevice(localcontrollerList.at(ui->xinputComboBox->currentIndex()));
    mapDiag.setMapping(localcontrollerMapping);
    if (mapDiag.exec() == QDialog::Accepted)
    {
        localcontrollerMapping = mapDiag.mapping();
    }
}


