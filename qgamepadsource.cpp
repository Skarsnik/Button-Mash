#include "qgamepadsource.h"

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
    if (deviceId != m_deviceId)
        return;
    if (butMapping.contains(button));
        emit buttonPressed(butMapping[button]);
}

void QGamepadSource::onGamepadButtonReleaseEvent(int deviceId, QGamepadManager::GamepadButton button)
{
    if (deviceId != m_deviceId)
        return;
    if (butMapping.contains(button));
        emit buttonReleased(butMapping[button]);
}

void QGamepadSource::onGamepadAxisEvent(int deviceId, QGamepadManager::GamepadAxis axis, double value)
{

}
