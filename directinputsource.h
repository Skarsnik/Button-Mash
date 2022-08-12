#ifndef DIRECTINPUTSOURCE_H
#define DIRECTINPUTSOURCE_H

#include <QMap>
#include <QTimer>

#include "inputprovider.h"
#include "localcontroller.h"
#include "QGameController/src/gamecontroller/qgamecontroller.h"

class DirectInputSource : public LocalController
{
public:
    DirectInputSource(int device);


public:
    void start();
    void stop();
    bool isReady();
    QString statusText();
    QString name() const;
    void    setMapping(QMap<InputProvider::SNESButton, LocalControllerButtonAxisInfos>);

private:
    QGameController*    controller;
    QTimer              timer;
    QMap<unsigned int, double> oldAxisValue;

    QMap<unsigned int, InputProvider::SNESButton>             buttonMapping;
    QMap<QPair<unsigned int, double>, InputProvider::SNESButton>    axisMapping;
    void    handleButtonEvent(QGameControllerButtonEvent* buttonEvent);
    void    handleAxisEvent(QGameControllerAxisEvent*   axisEvent);
};

#endif // DIRECTINPUTSOURCE_H
