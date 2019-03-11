#include "snesclassictelnet.h"

static const QString SNESClassicIP = "169.254.13.37";

SNESClassicTelnet::SNESClassicTelnet()
{
    controlCo = new TelnetConnection(SNESClassicIP, 23, "root", "clover");
    controlCo->conneect();
    decoder = new InputDecoder();
    connect(decoder, &InputDecoder::buttonPressed, this, &SNESClassicTelnet::buttonPressed);
    connect(decoder, &InputDecoder::buttonReleased, this, &SNESClassicTelnet::buttonReleased);
}


void SNESClassicTelnet::start()
{
    inputCo = new TelnetConnection(SNESClassicIP, 23, "root", "clover");
    inputCo->debugName = "Input";
    inputCo->setOneCommandMode(true);
    inputCo->conneect();
    connect(inputCo, &TelnetConnection::commandReturnedNewLine, this, &SNESClassicTelnet::onCommandReturned);
    connect(inputCo, &TelnetConnection::connected, this, &SNESClassicTelnet::onInputConnected);
}

void SNESClassicTelnet::stop()
{
    controlCo->executeCommand("killall hexdump");
    inputCo->close();
    inputCo->deleteLater();
}

bool SNESClassicTelnet::isReady()
{
    return controlCo->state() == TelnetConnection::Connected;
}

QString SNESClassicTelnet::name()
{
    return "SNES Classic";
}

QString SNESClassicTelnet::statusText()
{
    if (controlCo->state() != TelnetConnection::Connected)
    {
        return tr("Not connected to SNES Classic, be sure it's moded with hakchi2CE");
    } else {
        return tr("Connected to the SNES Classic");
    }
}

void SNESClassicTelnet::onInputConnected()
{
    inputCo->executeCommand("hexdump -v -e '32/1 \"%02X\" \"\\n\"' /dev/input/by-path/platform-twi.1-event-joystick");
}

void SNESClassicTelnet::onCommandReturned(QByteArray data)
{
    decoder->decodeHexdump(data);
}
