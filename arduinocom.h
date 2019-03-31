#ifndef ARDUINOCOM_H
#define ARDUINOCOM_H

#include <QObject>
#include <QSerialPort>
#include <QMap>
#include <QVector>
#include "inputprovider.h"


class ArduinoCOM : public InputProvider
{
    Q_OBJECT
public:
    explicit ArduinoCOM(QString port, QObject *parent = nullptr);
    void    configurePacket();
    void    setPort(QString port);
    void    setType(int type);

public slots:


private slots:
    void    portReadyRead();
private:

    enum Type {
        NintendoSpy = 1,
        Type2 = 2
    };
    QSerialPort comPort;
    QMap<quint16, InputProvider::SNESButton>  maskToButton;
    quint8      packetSize;
    QVector<InputProvider::SNESButton> nintendoSpyButtons;
    QVector<char>   NSButtonState;
    quint16     buttonState;
    int         m_type;
    void        processNintendoSpy(QByteArray data);

    // InputProvider interface
public:
    void start();
    void stop();
    bool isReady();
    QString statusText();
    QString name();
};

#endif // ARDUINOCOM_H
