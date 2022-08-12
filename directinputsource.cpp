#include <QDebug>
#include "directinputsource.h"

DirectInputSource::DirectInputSource(int device)
{
    controller = new QGameController(device);
    connect(controller, &QGameController::gameControllerButtonEvent, this, &DirectInputSource::handleButtonEvent);
    connect(controller, &QGameController::gameControllerAxisEvent, this, &DirectInputSource::handleAxisEvent);
    connect(&timer, &QTimer::timeout, controller, &QGameController::readGameController);
    timer.setInterval(10);
}


void DirectInputSource::start()
{
    timer.start();
}

void DirectInputSource::stop()
{
    timer.stop();
}

bool DirectInputSource::isReady()
{
    return controller->isValid();
}

QString DirectInputSource::statusText()
{
    return tr("Gamepad connected");
}

QString DirectInputSource::name() const
{
    return controller->description();
}

void DirectInputSource::setMapping(QMap<InputProvider::SNESButton, LocalControllerButtonAxisInfos> map)
{
    QMapIterator<InputProvider::SNESButton, LocalControllerButtonAxisInfos> it(map);
    buttonMapping.clear();
    axisMapping.clear();
    while (it.hasNext())
    {
        it.next();
        if (it.value().button != -1)
        {
            buttonMapping[it.value().button] = it.key();
        } else {
            axisMapping[QPair<unsigned int, double>(it.value().axis, it.value().value)] = it.key();
        }
    }
    qDebug() << buttonMapping;
    qDebug() << axisMapping;
}

void DirectInputSource::handleButtonEvent(QGameControllerButtonEvent *buttonEvent)
{
    if (buttonMapping.contains(buttonEvent->button()))
    {
        if (buttonEvent->pressed())
        {
            emit buttonPressed(buttonMapping[buttonEvent->button()]);
        } else {
            emit buttonReleased(buttonMapping[buttonEvent->button()]);
        }
    }
    delete buttonEvent;
}

void DirectInputSource::handleAxisEvent(QGameControllerAxisEvent *axisEvent)
{
    QMapIterator<QPair<unsigned int, double>, InputProvider::SNESButton> it(axisMapping);
    qDebug() << "Axis event" << axisEvent->axis() << axisEvent->value();
    while (it.hasNext())
    {
        it.next();
        if (it.key().first == axisEvent->axis())
        {
            qDebug() << it.value();
            if (axisEvent->value() == it.key().second)
            {
                qDebug() << "Axis " << axisEvent->axis() << " pressed";
                oldAxisValue[axisEvent->axis()] = axisEvent->value();
                emit buttonPressed(it.value());
            }
            if (axisEvent->value() != oldAxisValue[axisEvent->axis()]
             && it.key().second == oldAxisValue[axisEvent->axis()])
            {
                qDebug() << "Axis " << axisEvent->axis() << " released";
                oldAxisValue[axisEvent->axis()] = axisEvent->value();
                emit buttonReleased(it.value());
            }
        }
    }
    delete axisEvent;
}
