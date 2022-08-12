#ifndef LOCALCONTROLLER_H
#define LOCALCONTROLLER_H

#include <QMap>
#include "inputprovider.h"
#include "localcontrollermanager.h"


class LocalController : public InputProvider
{
public:
    LocalController();
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isReady() = 0;
    virtual QString statusText() = 0;
    virtual QString name() const = 0;
    virtual void    setMapping(LocalControllerMapping) = 0;
};

#endif // LOCALCONTROLLER_H
