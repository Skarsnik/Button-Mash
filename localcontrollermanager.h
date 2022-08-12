#ifndef LOCALCONTROLLERMANAGER_H
#define LOCALCONTROLLERMANAGER_H

#include <QStringList>
#include <QMap>
#include <QSettings>
#include "inputprovider.h"

#ifdef Q_OS_WIN
#include "QGameController/src/gamecontroller/qgamecontroller.h"
#endif

class LocalController;

struct LocalControllerButtonAxisInfos
{
    qint64  button;
    qint64  axis;
    double  value;

    QString toString() const;
    static LocalControllerButtonAxisInfos fromString(QString string);
};

typedef QMap<InputProvider::SNESButton, LocalControllerButtonAxisInfos> LocalControllerMapping;

struct LocalControllerInfos
{
    QString name;
    QString id;
};

class LocalControllerManager
{
public:
    static LocalControllerManager   *getManager();
    QList<LocalControllerInfos>     listController();
    LocalController*                createProvider(QString id);
    void                            setMappings(InputProvider* provider, LocalControllerMapping mapping);
    LocalControllerMapping          loadMapping(QSettings &setting, QString settingPath);
    void                            saveMapping(QSettings &setting, QString settingPath, LocalControllerMapping mapping);

private:
    LocalControllerManager();
    static LocalControllerManager* instance;
};

#endif // LOCALCONTROLLERMANAGER_H
