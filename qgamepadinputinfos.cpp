#include "qgamepadinputinfos.h"


QString     QGamepadInputInfos::toString() const {
    QString toret = QString("%1;%2;%3").arg(button).arg(axis).arg(value);
    return toret;
}


QGamepadInputInfos QGamepadInputInfos::fromString(QString string)
{
    QGamepadInputInfos toret;

    QStringList sl = string.split(';');
    bool ok;
    toret.button = QGamepadManager::GamepadButton(sl.at(0).toInt(&ok));
    toret.axis = QGamepadManager::GamepadAxis(sl.at(1).toInt(&ok));
    toret.value = sl.at(2).toDouble();
    return toret;
}
