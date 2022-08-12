#include "qgamepadsource.h"
#include <QDebug>

QGamepadSource::QGamepadSource(int device)
{
    m_deviceId = device;
    QGamepadManager* gp = QGamepadManager::instance();
    connect(gp, &QGamepadManager::gamepadButtonPressEvent, this, &QGamepadSource::onGamepadButtonPressEvent);
    connect(gp, &QGamepadManager::gamepadButtonReleaseEvent, this, &QGamepadSource::onGamepadButtonReleaseEvent);
    connect(gp, &QGamepadManager::gamepadAxisEvent, this, &QGamepadSource::onGamepadAxisEvent);
}

void QGamepadSource::onGamepadButtonPressEvent(int deviceId, QGamepadManager::GamepadButton button, double value)
{
    qDebug() << "Pressed : " << button;
    if (deviceId != m_deviceId)
        return;
    if (buttonMapping.contains(button));
        emit buttonPressed(buttonMapping[button]);
}

void QGamepadSource::onGamepadButtonReleaseEvent(int deviceId, QGamepadManager::GamepadButton button)
{
    if (deviceId != m_deviceId)
        return;
    if (buttonMapping.contains(button));
        emit buttonReleased(buttonMapping[button]);
}

void QGamepadSource::onGamepadAxisEvent(int deviceId, QGamepadManager::GamepadAxis axis, double value)
{

}


void QGamepadSource::start()
{
}

void QGamepadSource::stop()
{
}

bool QGamepadSource::isReady()
{
    QGamepadManager* gp = QGamepadManager::instance();
    return gp->connectedGamepads().indexOf(m_deviceId) != -1;
}

QString QGamepadSource::statusText()
{
    return tr("Gamepad connected");
}

QString QGamepadSource::name() const
{
    return QGamepadManager::instance()->gamepadName(m_deviceId);
}

void QGamepadSource::setMapping(LocalControllerMapping map)
{
    QMapIterator<InputProvider::SNESButton, LocalControllerButtonAxisInfos> it(map);
    buttonMapping.clear();
    axisMapping.clear();
    while (it.hasNext())
    {
        it.next();
        if (it.value().button != QGamepadManager::ButtonInvalid)
            buttonMapping[QGamepadManager::GamepadButton(it.value().button)] = it.key();
    }
}
