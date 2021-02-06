#ifndef QGAMEPADINPUTINFOS_H
#define QGAMEPADINPUTINFOS_H

#include <QGamepadManager>

struct QGamepadInputInfos {
    QGamepadManager::GamepadButton  button;
    QGamepadManager::GamepadAxis    axis;
    double                          value;

    QString     toString() const;
    static QGamepadInputInfos fromString(QString string);
};


#endif // QGAMEPADINPUTINFOS_H
