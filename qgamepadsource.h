#ifndef XINPUTSOURCE_H
#define XINPUTSOURCE_H

#include "inputprovider.h"

#include <QGamepadManager>
#include <QObject>

class QGamepadSource : public InputProvider
{
public:
    QGamepadSource(int device);
    void start();
    void stop();
    bool isReady();
    QString statusText();
    QString name();


private slots:
    void    onGamepadButtonPressEvent(int deviceId, QGamepadManager::GamepadButton button, double value);
    void    onGamepadButtonReleaseEvent(int deviceId, QGamepadManager::GamepadButton button);

    void    onGamepadAxisEvent(int deviceId, QGamepadManager::GamepadAxis axis, double value);


private:
    int m_deviceId;
    QMap<QGamepadManager::GamepadButton, InputProvider::SNESButton> butMapping;
    QMap<QPair<QGamepadManager::GamepadAxis, double>, InputProvider::SNESButton>    axisMapping;  

};

#endif // XINPUTSOURCE_H
