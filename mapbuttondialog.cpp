#include "mapbuttondialog.h"
#include "ui_mapbuttondialog.h"

MapButtonDialog::MapButtonDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MapButtonDialog)
{
    ui->setupUi(this);
    setMode = false;
    QGamepadManager* mag = QGamepadManager::instance();
    connect(mag, &QGamepadManager::gamepadButtonPressEvent, this, &MapButtonDialog::onGamepadButtonPressEvent);
}

MapButtonDialog::~MapButtonDialog()
{
    delete ui;
}

void MapButtonDialog::on_upButton_clicked()
{
    m_currentButton = ui->upButton;
    butVal = InputProvider::Up;
    setMode = true;
}

void MapButtonDialog::onGamepadButtonPressEvent(int deviceId, QGamepadManager::GamepadButton button, double value)
{
    if (setMode)
    {
        QGamepadInputInfos info;
        info.button = button;
        info.axis = QGamepadManager::AxisInvalid;
        info.value = value;
        mapping[butVal] = info;
        setMode = false;
        m_currentButton->setText(buttonToText(info));
    }
}

void MapButtonDialog::onGamepadButtonReleaseEvent(int deviceId, QGamepadManager::GamepadButton button)
{

}

void MapButtonDialog::onGamepadAxisEvent(int deviceId, QGamepadManager::GamepadAxis axis, double value)
{
    if (setMode)
    {
        QGamepadInputInfos info;
        info.button = QGamepadManager::ButtonInvalid;
        info.axis = axis;
        info.value = value;
        mapping[butVal] = info;
        setMode = false;
    }
}

QString MapButtonDialog::buttonToText(MapButtonDialog::QGamepadInputInfos info)
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
    if (info.button != QGamepadManager::ButtonInvalid)
        return nameMap[info.button];
    return "Nop";
}
