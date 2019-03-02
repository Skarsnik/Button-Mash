#ifndef USB2SNESSOURCE_H
#define USB2SNESSOURCE_H

#include "inputprovider.h"
#include "usb2snes.h"

#include <QObject>

class Usb2SnesSource : public InputProvider
{
public:
    Usb2SnesSource(USB2snes* usb);
    void    start();

private slots:
    void    onTimerTick();
private:
    USB2snes*       usb2snes;
    unsigned int    address;
    QTimer          timer;

};

#endif // USB2SNESSOURCE_H
