#ifndef INPUTSOURCESELECTOR_H
#define INPUTSOURCESELECTOR_H

#include <QGamepad>
#include <QWidget>
#include "usb2snes.h"

namespace Ui {
class InputSourceSelector;
}

class InputSourceSelector : public QWidget
{
    Q_OBJECT

public:
    explicit InputSourceSelector(QWidget *parent = nullptr);
    ~InputSourceSelector();
    void    scanDevices();


private slots:
    void    onUsb2SnesConnected();

private:
    Ui::InputSourceSelector *ui;
    USB2snes*   usb2snes;


    void    activateSnesClassicTelnet();
    void    activateSnesClassicStuff();
    void    activateUsb2SnesStuff();
    void    setArduinoInfo();
    void    setXInputDevices();
};

#endif // INPUTSOURCESELECTOR_H
