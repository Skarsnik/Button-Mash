#include <QDebug>
#include "arduinocom.h"

ArduinoCOM::ArduinoCOM(QString port, QObject *parent)
{
    comPort.setPortName(port);
    connect(&comPort, &QSerialPort::readyRead, this, &ArduinoCOM::portReadyRead);

    packetSize = 3;
    buttonState = 0;
    configurePacket();
    m_type = NintendoSpy;
}

void ArduinoCOM::configurePacket()
{
    maskToButton[0x0001] = InputProvider::SNESButton::A;
    maskToButton[0x0100] = InputProvider::SNESButton::B;
    maskToButton[0x0002] = InputProvider::SNESButton::X;
    maskToButton[0x0200] = InputProvider::SNESButton::Y;
    maskToButton[0x0800] = InputProvider::SNESButton::Start;
    maskToButton[0x0400] = InputProvider::SNESButton::Select;
    maskToButton[0x0004] = InputProvider::SNESButton::L;
    maskToButton[0x0008] = InputProvider::SNESButton::R;
    maskToButton[0x4000] = InputProvider::SNESButton::Left;
    maskToButton[0x8000] = InputProvider::SNESButton::Right;
    maskToButton[0x1000] = InputProvider::SNESButton::Up;
    maskToButton[0x2000] = InputProvider::SNESButton::Down;

}

void ArduinoCOM::setPort(QString port)
{
    comPort.setPortName(port);
}

void ArduinoCOM::setType(int type)
{
    m_type = type;
}

/* FDFF = Y pressed 1111 1101
 FFFD = X
 FFFE = A
 FEFF = B
 BFFF = < 1011
 7FFF = > 0111
 DFFF = v 1101
 EFFF = ^ 1110
 FFFB = L
 FFF7 = R
 FBFF = start
 F7FF = select

00000001 = FFFFFFF0

XXX0XXX0 ? FFFFFFF0 =

XXXE xor 0001 = FFF0

 */


void    ArduinoCOM::processNintendoSpy(QByteArray data)
{
    qDebug() << "BLOCK" << data.toHex();
}

// Blue, .,  Red, , Yellow,


void ArduinoCOM::portReadyRead()
{
    static QByteArray dataRead;
    QByteArray data = comPort.readAll();
    dataRead.append(data);
    qDebug() << "Received " << data.size();
    if (m_type == Type::NintendoSpy)
    {
        while (!dataRead.isEmpty())
        {
            //qDebug() << dataRead.toHex();
            int nextSplit = dataRead.indexOf('\n');
            if (nextSplit == -1)
                break;
            qDebug() << nextSplit;
            processNintendoSpy(dataRead.left(nextSplit));
            dataRead.remove(0, nextSplit + 1);
        }
    }
    return ;
    if (data.size() > packetSize)
        return;

    if (dataRead.size() == packetSize)
    {
        /*if (dataRead != QByteArray::fromHex("FFFFFF"))
            qDebug() << dataRead;*/
        quint16 byte2 = ((uchar)dataRead.at(0) << 8) | (uchar)(dataRead.at(1));
        byte2 = ~byte2;
        //qDebug() << dataRead << QString::number(byte2, 16);
        QMapIterator<quint16, InputProvider::SNESButton> it(maskToButton);
        //qDebug() << QString::number(buttonState, 16);
        while (it.hasNext())
        {
            it.next();
            quint16 mask = it.key();
            //qDebug() << it.value() << QString::number(mask, 16) << QString::number(static_cast<quint16>(~mask), 16);
            if ((byte2 & mask) == mask)
            {
                if ((buttonState & mask) != mask)
                {
                    qDebug() << it.value() << "pressed";
                    emit buttonPressed(it.value());
                    buttonState = buttonState | mask;
                }
            } else {
                if ((buttonState & mask) == mask)
                {
                    buttonState = buttonState ^ mask;
                    qDebug() << it.value() << "released";
                    emit buttonReleased(it.value());
                }
            }
        }
        dataRead.clear();
    }
}


void ArduinoCOM::start()
{
    qDebug() << "opening port connection" << comPort.open(QIODevice::ReadWrite);
    comPort.clear();
    comPort.setBaudRate(115200);
    comPort.setDataTerminalReady(true);
    qDebug() << "BaudRate : " << comPort.baudRate();
    qDebug() << "Databits : " << comPort.dataBits();
    qDebug() << "DataTerminalReady : " << comPort.isDataTerminalReady();
    qDebug() << "Parity : " << comPort.parity();
    qDebug() << "FlowControl : " << comPort.flowControl();
    qDebug() << "Stop bits : " << comPort.stopBits();
}

void ArduinoCOM::stop()
{
    comPort.close();
}

bool ArduinoCOM::isReady()
{
    return true;
}

QString ArduinoCOM::statusText()
{
    return "Ready";
}

QString ArduinoCOM::name()
{
    return "Arduino " + comPort.portName();
}
