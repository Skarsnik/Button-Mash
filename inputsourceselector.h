#ifndef INPUTSOURCESELECTOR_H
#define INPUTSOURCESELECTOR_H

#include <QAbstractButton>
#include <QDialog>
#include <QGamepad>
#include <QWidget>
#include "arduinocom.h"
#include "inputprovider.h"
#include "snesclassictelnet.h"
#include "usb2snes.h"
#include "usb2snessource.h"

namespace Ui {
class InputSourceSelector;
}

class InputSourceSelector : public QDialog
{
    Q_OBJECT

public:
    explicit InputSourceSelector(QWidget *parent = nullptr);
    ~InputSourceSelector();
    InputProvider*  currentProvider();
    InputProvider*  getLastProvider();
    void    scanDevices();


private slots:
    void    onUsb2SnesConnected();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_refreshButton_clicked();

    void    onSourceButtonClicked(QAbstractButton *but);

    void on_arduinoComComboBox_currentIndexChanged(const QString &arg1);

    void on_usb2snesComboBox_currentTextChanged(const QString &arg1);

    void on_usb2gameComboBox_currentTextChanged(const QString &arg1);

private:
    Ui::InputSourceSelector *ui;
    USB2snes*           usb2snes;
    InputProvider*      m_currentProvider;
    SNESClassicTelnet*  snesClassicTelnet;
    ArduinoCOM*         arduinoCom;
    Usb2SnesSource*     usb2snesProvider;

    void    activateSnesClassicTelnet();
    void    activateSnesClassicStuff();
    void    activateUsb2SnesStuff();
    void    setArduinoInfo();
    void    setXInputDevices();

    // QDialog interface
public slots:
    int exec();
};

#endif // INPUTSOURCESELECTOR_H
