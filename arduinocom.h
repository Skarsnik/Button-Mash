#ifndef ARDUINOCOM_H
#define ARDUINOCOM_H

#include <QObject>
#include <QSerialPort>
#include <QMap>
#include "../inputdecoder.h"


class ArduinoCOM : public QObject
{
    Q_OBJECT
public:
    explicit ArduinoCOM(QString port, QObject *parent = nullptr);
    void    configurePacket();

signals:
    void    buttonPressed(InputDecoder::SNESButton button);
    void    buttonReleased(InputDecoder::SNESButton button);

public slots:


private slots:
    void    portReadyRead();
private:
    QSerialPort comPort;
    QMap<quint16, InputDecoder::SNESButton>  maskToButton;
    quint8      packetSize;
    quint16     buttonState;
};

#endif // ARDUINOCOM_H
