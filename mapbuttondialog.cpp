#include "mapbuttondialog.h"
#include "ui_mapbuttondialog.h"

#include <QDebug>

#include <QButtonGroup>

MapButtonDialog::MapButtonDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MapButtonDialog)
{
    ui->setupUi(this);
    setMode = false;
    connect(ui->mappingButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onButtonGroupClicked(QAbstractButton*)));
    ui->mappingButtonGroup->setId(ui->upButton, InputProvider::Up);
    ui->mappingButtonGroup->setId(ui->downButton, InputProvider::Down);
    ui->mappingButtonGroup->setId(ui->leftButton, InputProvider::Left);
    ui->mappingButtonGroup->setId(ui->rightButton, InputProvider::Right);
    ui->mappingButtonGroup->setId(ui->aButton, InputProvider::A);
    ui->mappingButtonGroup->setId(ui->bButton, InputProvider::B);
    ui->mappingButtonGroup->setId(ui->xButton, InputProvider::X);
    ui->mappingButtonGroup->setId(ui->yButton, InputProvider::Y);
    ui->mappingButtonGroup->setId(ui->lButton, InputProvider::L);
    ui->mappingButtonGroup->setId(ui->rButton, InputProvider::R);
    ui->mappingButtonGroup->setId(ui->startButton, InputProvider::Start);
    ui->mappingButtonGroup->setId(ui->selectButton, InputProvider::Select);
    directInputDevice_id = -1;
    directInputDevice = nullptr;
    qGamepadDevice_id = -1;
    directInputTimer.setInterval(10);

}

void MapButtonDialog::setDevice(LocalControllerInfos device)
{
    directInputDevice_id = -1;
    qGamepadDevice_id = -1;
    directInputTimer.stop();
    ui->nameLabel->setText(device.name);
    if (device.id.startsWith("DirectInput"))
    {
        qDebug() << "Setting Direct Input";
        directInputDevice_id = device.id.split(" ").at(1).toInt();
        directInputDevice = new QGameController(directInputDevice_id);
        qDebug() << directInputDevice->isValid();
        connect(directInputDevice, &QGameController::gameControllerButtonEvent, this, &MapButtonDialog::onDirectInputButtonEvent);
        connect(directInputDevice, &QGameController::gameControllerAxisEvent, this, &MapButtonDialog::onDirectInputAxisEvent);
        connect(&directInputTimer, &QTimer::timeout, directInputDevice, &QGameController::readGameController);
        directInputTimer.start();
    }
    if (device.id.startsWith("QGamepad"))
    {
        qGamepadDevice_id = device.id.split(" ").at(1).toInt();
        QGamepadManager* mag = QGamepadManager::instance();
        connect(mag, &QGamepadManager::gamepadButtonPressEvent, this, &MapButtonDialog::onGamepadButtonPressEvent);
        connect(mag, &QGamepadManager::gamepadAxisEvent, this, &MapButtonDialog::onGamepadAxisEvent);
    }
}

void MapButtonDialog::setMapping(QMap<InputProvider::SNESButton, LocalControllerButtonAxisInfos> mapping)
{
    m_mapping = mapping;

    QMapIterator<InputProvider::SNESButton, LocalControllerButtonAxisInfos>it(mapping);
    while (it.hasNext())
    {
        it.next();
        ui->mappingButtonGroup->button(it.key())->setText(buttonToText(it.value()));
    }
}

QMap<InputProvider::SNESButton, LocalControllerButtonAxisInfos> MapButtonDialog::mapping() const
{
    return m_mapping;
}

MapButtonDialog::~MapButtonDialog()
{
    if (directInputDevice != nullptr)
        delete directInputDevice;
    delete ui;
}


void MapButtonDialog::onButtonGroupClicked(QAbstractButton *button)
{
    m_currentButton = qobject_cast<QPushButton*>(button);
    butVal = InputProvider::SNESButton(ui->mappingButtonGroup->id(m_currentButton));
    m_currentButton->setFlat(false);
    setMode = true;
}

void MapButtonDialog::onGamepadButtonPressEvent(int deviceId, QGamepadManager::GamepadButton button, double value)
{
    if (setMode)
    {
        LocalControllerButtonAxisInfos info;
        info.button = button;
        info.axis = QGamepadManager::AxisInvalid;
        info.value = value;
        m_mapping[butVal] = info;
        setMode = false;
        qDebug() << m_mapping.keys();
        m_currentButton->setText(buttonToText(info));
        m_currentButton->setFlat(true);
    }
}

void MapButtonDialog::onGamepadButtonReleaseEvent(int deviceId, QGamepadManager::GamepadButton button)
{

}

void MapButtonDialog::onGamepadAxisEvent(int deviceId, QGamepadManager::GamepadAxis axis, double value)
{
    if (setMode)
    {
        LocalControllerButtonAxisInfos info;
        info.button = QGamepadManager::ButtonInvalid;
        info.axis = axis;
        info.value = value;
        m_mapping[butVal] = info;
        m_currentButton->setText(buttonToText(info));
        m_currentButton->setFlat(true);
        setMode = false;
    }
}

void MapButtonDialog::onDirectInputButtonEvent(QGameControllerButtonEvent *event)
{
    qDebug() << "Direct Input event" << event->pressed();
    if (setMode)
    {
        if (event->pressed() && event->controllerId() == directInputDevice_id)
        {
            LocalControllerButtonAxisInfos info;
            info.button = event->button();
            info.axis = -1;
            info.value = directInputDevice->buttonValue(info.button);
            m_mapping[butVal] = info;
            m_currentButton->setText(buttonToText(info));
            m_currentButton->setFlat(true);
            setMode = false;
        }
    }
}

void MapButtonDialog::onDirectInputAxisEvent(QGameControllerAxisEvent *event)
{
    if (setMode)
    {
        if (event->controllerId() == directInputDevice_id)
        {
            LocalControllerButtonAxisInfos info;
            info.button = -1;
            info.axis = event->axis();
            info.value = event->value();
            m_mapping[butVal] = info;
            m_currentButton->setText(buttonToText(info));
            m_currentButton->setFlat(true);
            setMode = false;
        }
    }
}

QString MapButtonDialog::buttonToText(LocalControllerButtonAxisInfos info)
{
    QMap<QGamepadManager::GamepadButton, QString> nameMap;
    nameMap[QGamepadManager::ButtonA] = "A";
    nameMap[QGamepadManager::ButtonB] = "B";
    nameMap[QGamepadManager::ButtonX] = "X";
    nameMap[QGamepadManager::ButtonY] = "Y";
    nameMap[QGamepadManager::ButtonL1] = "L1";
    nameMap[QGamepadManager::ButtonL2] = "L2";
    nameMap[QGamepadManager::ButtonL3] = "L3";
    nameMap[QGamepadManager::ButtonR1] = "R1";
    nameMap[QGamepadManager::ButtonR2] = "R2";
    nameMap[QGamepadManager::ButtonR3] = "R3";
    nameMap[QGamepadManager::ButtonStart] = "Start";
    nameMap[QGamepadManager::ButtonSelect] = "Select";
    nameMap[QGamepadManager::ButtonUp] = "Up";
    nameMap[QGamepadManager::ButtonDown] = "Down";
    nameMap[QGamepadManager::ButtonLeft] = "Left";
    nameMap[QGamepadManager::ButtonRight] = "Right";
    nameMap[QGamepadManager::ButtonCenter] = "Center";
    nameMap[QGamepadManager::ButtonGuide] = "Guide";
    if (qGamepadDevice_id != -1 && info.button != QGamepadManager::ButtonInvalid)
        return nameMap[QGamepadManager::GamepadButton(info.button)];
    if (directInputDevice_id != -1 && info.button != -1)
    {
        return QString(tr("Button %1").arg(info.button));
    }
    if (directInputDevice_id != -1 && info.axis != -1)
    {
        return QString(tr("Axis %1%2").arg(info.axis).arg(info.value >= 0 ? '+' : '-'));
    }
    return "Nop";
}
