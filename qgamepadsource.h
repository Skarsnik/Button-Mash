#ifndef XINPUTSOURCE_H
#define XINPUTSOURCE_H

#include "localcontroller.h"
#include "qgamepadinputinfos.h"

#include <QGamepadManager>
#include <QObject>

class QGamepadSource : public LocalController
{
public:
    QGamepadSource(int device);
    void start();
    void stop();
    bool isReady();
    QString statusText();
    QString name() const;
    void setMapping(LocalControllerMapping mapping);

private slots:
    void    onGamepadButtonPressEvent(int deviceId, QGamepadManager::GamepadButton button, double value);
    void    onGamepadButtonReleaseEvent(int deviceId, QGamepadManager::GamepadButton button);

    void    onGamepadAxisEvent(int deviceId, QGamepadManager::GamepadAxis axis, double value);


private:
    int m_deviceId;
    QMap<QGamepadManager::GamepadButton, InputProvider::SNESButton>                 buttonMapping;
    QMap<QPair<QGamepadManager::GamepadAxis, double>, InputProvider::SNESButton>    axisMapping;

};

#endif // XINPUTSOURCE_H
