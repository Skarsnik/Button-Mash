#include <QDebug>
#include "arduinocom.h"

ArduinoCOM::ArduinoCOM(QString port, QObject *parent) : QObject(parent)
{
    comPort.setPortName(port);
    connect(&comPort, &QSerialPort::readyRead, this, &ArduinoCOM::portReadyRead);
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
    packetSize = 3;
    buttonState = 0;
    configurePacket();
}

void ArduinoCOM::configurePacket()
{
    maskToButton[0x0001] = InputDecoder::SNESButton::A;
    maskToButton[0x0100] = InputDecoder::SNESButton::B;
    maskToButton[0x0002] = InputDecoder::SNESButton::X;
    maskToButton[0x0200] = InputDecoder::SNESButton::Y;
    maskToButton[0x0800] = InputDecoder::SNESButton::Start;
    maskToButton[0x0400] = InputDecoder::SNESButton::Select;
    maskToButton[0x0004] = InputDecoder::SNESButton::L;
    maskToButton[0x0008] = InputDecoder::SNESButton::R;
    maskToButton[0x4000] = InputDecoder::SNESButton::Left;
    maskToButton[0x8000] = InputDecoder::SNESButton::Right;
    maskToButton[0x1000] = InputDecoder::SNESButton::Up;
    maskToButton[0x2000] = InputDecoder::SNESButton::Down;

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



void ArduinoCOM::portReadyRead()
{
    static QByteArray dataRead;
    QByteArray data = comPort.readAll();
    if (data.size() > packetSize)
        return;
    dataRead.append(data);
    if (dataRead.size() == packetSize)
    {
        /*if (dataRead != QByteArray::fromHex("FFFFFF"))
            qDebug() << dataRead;*/
        quint16 byte2 = ((uchar)dataRead.at(0) << 8) | (uchar)(dataRead.at(1));
        byte2 = ~byte2;
        //qDebug() << dataRead << QString::number(byte2, 16);
        QMapIterator<quint16, InputDecoder::SNESButton> it(maskToButton);
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
