#include <QMetaEnum>
#include <QDebug>

#include "directinputsource.h"
#include "localcontrollermanager.h"
#include "qgamepadsource.h"
#include <QGamepadManager>


LocalControllerManager* LocalControllerManager::instance = nullptr;

LocalControllerManager *LocalControllerManager::getManager()
{
    if (instance == nullptr)
        instance = new LocalControllerManager();
    return instance;
}

QList<LocalControllerInfos> LocalControllerManager::listController()
{
    QList<LocalControllerInfos> toret;
    auto list = QGamepadManager::instance()->connectedGamepads();
    for (auto id : list)
    {
        LocalControllerInfos info;
        info.name = QGamepadManager::instance()->gamepadName(id);
        info.id = "QGamepad " + QString::number(id);
        toret << info;
    }
#ifdef Q_OS_WIN
    for (quint8 i = 0; i < 10; i++)
    {
        QGameController controller(i);
        if (controller.isValid())
        {
            LocalControllerInfos info;
            info.name = controller.description();
            info.id = "DirectInput " + QString::number(i);
            toret << info;
        }
    }
#endif
    return toret;
}

LocalController *LocalControllerManager::createProvider(QString id)
{
    qDebug() << "Creating new Localcontroller " << id;
#ifdef Q_OS_WIN
    if (id.startsWith("DirectInput"))
        return new DirectInputSource(id.split(" ").at(1).toUInt());
#endif
    if (id.startsWith("QGamepad"))
        return new QGamepadSource(id.split(" ").at(1).toUInt());
    return nullptr;
}

LocalControllerMapping LocalControllerManager::loadMapping(QSettings &setting, QString settingPath)
{
    LocalControllerMapping toret;

    QMetaEnum snesButtonMeta = QMetaEnum::fromType<InputProvider::SNESButton>();
    for (unsigned int i = 0; i < snesButtonMeta.keyCount(); i++)
    {
        const char* keyName = snesButtonMeta.key(i);
        if (setting.contains(settingPath + "/Button" + keyName))
        {
            toret[InputProvider::SNESButton(snesButtonMeta.value(i))]
                    = LocalControllerButtonAxisInfos::fromString(setting.value(settingPath + "/Button" + keyName).toString());
        }
    }
    return toret;
}

void LocalControllerManager::saveMapping(QSettings &setting, QString settingPath, LocalControllerMapping mapping)
{
    QMapIterator<InputProvider::SNESButton, LocalControllerButtonAxisInfos> it(mapping);
    QMetaEnum snesButtonMeta = QMetaEnum::fromType<InputProvider::SNESButton>();
    while (it.hasNext())
    {
        it.next();
        auto button = it.key();
        auto map = it.value();
        const char* buttonName = snesButtonMeta.valueToKey(button);
        setting.setValue(settingPath + "/Button" + buttonName, map.toString());
    }
}

LocalControllerManager::LocalControllerManager()
{

}

QString LocalControllerButtonAxisInfos::toString() const
{
    QString toret = QString("%1;%2;%3").arg(button).arg(axis).arg(value);
    return toret;
}

LocalControllerButtonAxisInfos LocalControllerButtonAxisInfos::fromString(QString string)
{
    LocalControllerButtonAxisInfos toret;
    QStringList sl = string.split(";");
    bool ok;
    toret.button = sl.at(0).toLongLong(&ok);
    toret.axis = sl.at(1).toLongLong(&ok);
    toret.value = sl.at(2).toDouble(&ok);
    return toret;
}
