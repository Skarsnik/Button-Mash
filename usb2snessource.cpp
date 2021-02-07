#include "usb2snessource.h"

static const unsigned int defaultAddress = 0xF90718;

Usb2SnesSource::Usb2SnesSource(USB2snes* usb)
{
    QList<quint32> l;
    l << defaultAddress;
    address = l;
    timer.setInterval(30);
    connect(&timer, &QTimer::timeout, this, &Usb2SnesSource::onTimerTick);
    usb2snes = usb;
    connect(usb2snes, &USB2snes::connected, this, &Usb2SnesSource::onUsb2SnesConnected, Qt::UniqueConnection);
}


void Usb2SnesSource::onUsb2SnesConnected()
{
    if (!device.isEmpty())
        usb2snes->attach(device);
}

void Usb2SnesSource::start()
{
    timer.start();
}

void Usb2SnesSource::stop()
{
    timer.stop();
    usb2snes->disconnect();
    disconnect(usb2snes, &USB2snes::connected, this, nullptr);
}

bool Usb2SnesSource::isReady()
{
    if (usb2snes->state() == USB2snes::None)
    {
        usb2snes->connect();
        connect(usb2snes, &USB2snes::connected, this, &Usb2SnesSource::onUsb2SnesConnected, Qt::UniqueConnection);
    }
    if (usb2snes->state() == USB2snes::Connected)
    {
        if (!device.isEmpty())
            usb2snes->attach(device);
    }
    return usb2snes->state() == USB2snes::Ready;
}

QString Usb2SnesSource::name() const
{
    return "Usb2Snes";
}

QString Usb2SnesSource::statusText()
{
    if (usb2snes->state() == USB2snes::None)
        return "Not connected";
    if (usb2snes->state() == USB2snes::Connected)
        return "Connected";
    return "I just don't know what went wrong!";
}

void Usb2SnesSource::setDevice(QString adevice)
{
    device = adevice;
}

QStringList Usb2SnesSource::loadGamesList()
{
    QStringList toret;
    toret.append("Default");
    lookupAddresses.clear();
    lookupAddresses["Default"].append(defaultAddress);
    QFile fi(qApp->applicationDirPath() + "/usb2snesgames.txt");
    if (fi.open(QIODevice::Text | QIODevice::ReadOnly))
    {
        while (!fi.atEnd())
        {
            QString s = fi.readLine();
            auto name = s.split('=').at(0);
            auto addrs = s.split('=').at(1).split(',');
            toret << name;
            bool ok;
            foreach(QString na, addrs) {
               lookupAddresses[name].append(na.toUInt(&ok, 16));
            }
        }
    }
    return toret;
}

bool Usb2SnesSource::setGame(QString game)
{
    qDebug() << "Loading profile for " << game;
    if (lookupAddresses.contains(game))
    {
        address = lookupAddresses[game];
        return true;
    }
    return false;
}

void Usb2SnesSource::onTimerTick()
{
    static quint16 prev_input = 0;
    QByteArray input;
    if (address.size() == 1)
    {
        /*if (address.at(0) == defaultAddress)
            input = usb2snes->getAddress(defaultAddress, 2);
        else*/
            input = usb2snes->getAddress(address.at(0), 2);

    } else {
        QList<QPair<unsigned int, unsigned int> > l;
        l << QPair<unsigned int, unsigned int>(address.at(0), 1) << QPair<unsigned int, unsigned int>(address.at(1), 1);
        input = usb2snes->getAddress(l);
    }
    if (input.isEmpty())
        return ;
    QMap<quint16, InputProvider::SNESButton> maskToButton;
    maskToButton[0x1000] = Start;
    maskToButton[0x2000] = Select;
    maskToButton[0x0080] = A;
    maskToButton[0x8000] = B;
    maskToButton[0x4000] = Y;
    maskToButton[0x0040] = X;
    maskToButton[0x0020] = L;
    maskToButton[0x0010] = R;
    maskToButton[0x0100] = Right;
    maskToButton[0x0200] = Left;
    maskToButton[0x0400] = Down;
    maskToButton[0x0800] = Up;
    quint16 nInput = ((quint16) ((static_cast<uchar>(input.at(1)) << 8)) | ((quint16) static_cast<uchar>(input.at(0)) & 0x00FF));
    /*
    0101
    1100

    0100 ^ 1100 = 1000 // New press
    0100 ^ 0101 = 0001 // release */
    quint16 released = (prev_input & nInput) ^ prev_input;
    quint16 pressed = (prev_input & nInput) ^ nInput;
    foreach(quint16 mask, maskToButton.keys())
    {
            if ((mask & released) == mask)
                emit buttonReleased(maskToButton[mask]);
            if ((mask & pressed) == mask)
                emit buttonPressed(maskToButton[mask]);
    }
    prev_input = nInput;
}
