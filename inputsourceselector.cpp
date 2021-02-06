#include "inputsourceselector.h"
#include "mapbuttondialog.h"
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
const QString SETTING_USB2SNES = "Usb2Snes";
const QString SETTING_USB2SNES_DEVICE =  "Usb2SnesDevice";
const QString SETTING_USB2SNES_GAME =  "Usb2SnesGame";
const QString SETTING_ARDUINO = "Arduino";
const QString SETTING_ARDUINOCOM = "ArduinoCOM";
const QString SETTING_DIRECT_INPUT = "DirectInput";
const QString SETTING_QGAMEPAD_INPUT = "QGamepad";
const QString SETTING_QGAMEPAD_MAPPING = "QGamepad/mapping";
const QString SETTING_QGAMEPAD_DEVICEID = "QGamepaddeviceid";

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
    qgamepadProvider = nullptr;
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
        if (inputSource == SETTING_QGAMEPAD_INPUT)
        {
            qgamepadProvider = new QGamepadSource(globalSetting->value("inputSource/" + SETTING_QGAMEPAD_DEVICEID).toInt());
            qgamepadMapping = loadQGamepadMapping();
            qgamepadProvider->setMapping(qgamepadMapping);
            m_currentProvider = qgamepadProvider;
        }
        return m_currentProvider;
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
    if (!gamepads.isEmpty())
        setQGamepads();
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

void InputSourceSelector::setQGamepads()
{
    ui->xinputComboBox->setEnabled(true);
    ui->xinputRadioButton->setEnabled(true);
    ui->xinputComboBox->clear();
    auto gamepads = QGamepadManager::instance()->connectedGamepads();
    unsigned int cpt = 0;
    foreach(int id, gamepads)
    {
        //QGamepad pad(id);
        qDebug() << id;
        qDebug() << QGamepadManager::instance()->gamepadName(id);
        ui->xinputComboBox->addItem(QString("XInput Device %1").arg(id));
        ui->xinputComboBox->setItemData(cpt, id, Qt::UserRole + 1);
        cpt++;
    }
}

QGamepadMapping InputSourceSelector::loadQGamepadMapping()
{
    QGamepadMapping toret;

    QMetaEnum snesButtonMeta = QMetaEnum::fromType<InputProvider::SNESButton>();
    for (unsigned int i = 0; i < snesButtonMeta.keyCount(); i++)
    {
        const char* keyName = snesButtonMeta.key(i);
        if (globalSetting->contains(SETTING_QGAMEPAD_MAPPING + "/Button" + keyName))
        {
            toret[InputProvider::SNESButton(snesButtonMeta.value(i))]
                    = QGamepadInputInfos::fromString(globalSetting->value(SETTING_QGAMEPAD_MAPPING + "/Button" + keyName).toString());
        }
    }
    return toret;
}


void        InputSourceSelector::saveQGamepadMapping(QGamepadMapping mapping)
{
    QMapIterator<InputProvider::SNESButton, QGamepadInputInfos> it(mapping);
    QMetaEnum snesButtonMeta = QMetaEnum::fromType<InputProvider::SNESButton>();
    while (it.hasNext())
    {
        it.next();
        auto button = it.key();
        auto map = it.value();
        const char* buttonName = snesButtonMeta.valueToKey(button);
        globalSetting->setValue(SETTING_QGAMEPAD_MAPPING + "/Button" + buttonName, map.toString());
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
    if (ui->usb2snesRadioButton->isChecked())
    {
        globalSetting->setValue(SETTING_INPUTSOURCE, SETTING_USB2SNES);
        globalSetting->setValue("inputSource/" + SETTING_USB2SNES_DEVICE, ui->usb2snesComboBox->currentText());
        globalSetting->setValue("inputSource/" + SETTING_USB2SNES_GAME, ui->usb2gameComboBox->currentText());
    }
    if (ui->xinputRadioButton->isChecked())
    {
        globalSetting->setValue(SETTING_INPUTSOURCE, SETTING_QGAMEPAD_INPUT);
        globalSetting->setValue("inputSource/" + SETTING_QGAMEPAD_DEVICEID, ui->xinputComboBox->currentData(Qt::UserRole + 1).toInt());
        saveQGamepadMapping(qgamepadMapping);
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
    if (but == ui->usb2snesRadioButton)
    {
        if (usb2snesProvider == nullptr)
            usb2snesProvider = new Usb2SnesSource(usb2snes);
        m_currentProvider = usb2snesProvider;

    }
    if (but == ui->xinputRadioButton)
    {
        ui->mappingButton->setEnabled(true);
        if (qgamepadProvider == nullptr)
            qgamepadProvider = new QGamepadSource(ui->xinputComboBox->currentData(Qt::UserRole + 1).toInt());
        qgamepadProvider->setMapping(qgamepadMapping);
        m_currentProvider = qgamepadProvider;
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

void InputSourceSelector::on_mappingButton_clicked()
{
    MapButtonDialog mapDiag;

    mapDiag.setMapping(qgamepadMapping);
    if (mapDiag.exec() == QDialog::Accepted)
    {
        qgamepadMapping = mapDiag.mapping();
    }
}
