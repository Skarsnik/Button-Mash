#ifndef USB2SNESSOURCE_H
#define USB2SNESSOURCE_H

#include <QObject>
#include "inputprovider.h"
#include "usb2snes.h"



class Usb2SnesSource : public InputProvider
{
    Q_OBJECT
public:
    Usb2SnesSource(USB2snes* usb);
    void    start();
    void    stop();
    bool    isReady();
    QString name() const;
    QString statusText();
    void    setDevice(QString device);
    QStringList    loadGamesList();
    bool           setGame(QString);

private slots:
    void    onTimerTick();
    void onUsb2SnesConnected();
private:
    USB2snes*       usb2snes;
    QList<unsigned int>    address;
    QTimer          timer;
    QString         device;
    QMap<QString, QList<quint32>>   lookupAddresses;
};

#endif // USB2SNESSOURCE_H
