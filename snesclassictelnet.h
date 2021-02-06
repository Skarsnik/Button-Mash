#ifndef SNESCLASSICTELNET_H
#define SNESCLASSICTELNET_H

#include <QObject>
#include "inputdecoder.h"
#include "inputprovider.h"
#include "telnetconnection.h"


class SNESClassicTelnet : public InputProvider
{
    Q_OBJECT
public:
    SNESClassicTelnet();
    void    start();
    void    stop();
    bool    isReady();
    QString name() const;
    QString statusText();


private slots:
    void                onInputConnected();
    void                onCommandReturned(QByteArray data);
private:
    TelnetConnection*   controlCo;
    TelnetConnection*   inputCo;
    InputDecoder*       decoder;

};

#endif // SNESCLASSICTELNET_H
