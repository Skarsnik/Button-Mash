#include "inputdecoder.h"
#include <QtEndian>
#include <QDebug>

InputDecoder::InputDecoder() : InputProvider ()
{
    Lpressed = false;
    Rpressed = false;
}

/*
93698 388871 3 2 0
93698 388883 1 310 1

93698 388888 0 0 0
93698 612196 3 2 0 */

/*

93738 985480 3 5 0
93738 985496 1 311 1

93738 985502 0 0 0
93739 258830 3 5 0
 * */


/*
   RAW: "F611000081690B00000000000000000006120000B8C302000100C20201000000\r\n"
RAW: "06120000C1C3020000000000000000000612000043E704000100C20200000000\r\n"
RAW: "061200004CE70400000000000000000007120000B0EB0E000100C10201000000\r\n"
RAW: "07120000BEEB0E00000000000000000008120000CB9801000100C10200000000\r\n"
RAW: "08120000D5980100000000000000000008120000FDE70D000100C30201000000\r\n"
RAW: "0812000006E80D00000000000000000009120000D8AD00000100C30200000000\r\n"
RAW: "09120000E3AD0000000000000000000009120000A06407000100C00201000000\r\n"
RAW: "09120000A8640700000000000000000009120000027B09000100C00200000000\r\n"
RAW: "091200000B7B090000000000000000000C120000C8D907000100310101000000\r\n"
RAW: "0C120000CFD9070000000000000000000C12000053C00A000100310100000000\r\n"
RAW: "0C1200005AC00A0000000000000000000C120000F4EB0E000100300101000000\r\n"
RAW: "0C120000FEEB0E0000000000000000000D120000B6B702000100300100000000\r\n"
RAW: "0D120000C3B7020000000000000000000D120000C2EF06000100330101000000\r\n"
RAW: "0D120000CBEF060000000000000000000D120000DCE209000100330100000000\r\n"
RAW: "0D120000E3E2090000000000000000000D120000EBC40E000100340101000000\r\n"
RAW: "0D120000F3C40E0000000000000000000E120000610D02000100340100000000\r\n"
RAW: "0E120000670D0200000000000000000011120000101F060001003B0101000000\r\n"
RAW: "11120000191F0600000000000000000011120000AD94090001003B0100000000\r\n"
RAW: "11120000B6940900000000000000000011120000E1A10C0001003A0101000000\r\n"
RAW: "11120000EBA10C000000000000000000121200002D7E010001003A0100000000\r\n"

*/
// >v<^ baxy st sl


// 99730200D0650400010030010000000099730200D86504000000000000000000

/*
struct input_event {
    struct timeval time;
    __u16 type;
    __u16 code;
    __s32 value;
};

struct timeval {
               time_t      tv_sec;
               suseconds_t tv_usec;
};

*/

RawInputEvent    stringtoRawInputEvent(QString str)
{
    RawInputEvent toret;
    //qDebug() << str << "code" << str.mid(20, 4) << "type : " << str.mid(16, 4);
    toret.time_s  =  qToBigEndian<quint32>(str.left(8).toUInt(NULL, 16));
    toret.time_us = qToBigEndian<quint32>(str.mid(8, 8).toUInt(NULL, 16));
    toret.type = qToBigEndian<quint16>(str.mid(16, 4).toUInt(NULL, 16));
    toret.code = qToBigEndian<quint16>(str.mid(20, 4).toUInt(NULL, 16));
    toret.value = qToBigEndian<qint32>(str.mid(24, 8).toInt(NULL, 16));
    return toret;
}

void    InputDecoder::processEvent(RawInputEvent ev)
{
    //qDebug() << ev.time_s << ev.time_us << ev.type << ev.code << ev.value;
    //Type 0 is a an empty event to separate event
    if (ev.type == 0x00)
        return ;
    if (ev.type == 0x01) // 01 is standard key press/release event
    {
        if (ev.code == 311 || ev.code == 310)
            return;
        if (ev.value == 1)
            emit buttonPressed((InputDecoder::SNESButton)ev.code);
        if (ev.value == 0)
            emit buttonReleased((InputDecoder::SNESButton)ev.code);

    }
    if (ev.type == 0x03) // for L and R
    {
        if (ev.code == 2)
        {
            if (Lpressed)
                emit buttonReleased((InputDecoder::SNESButton)ev.code);
            else
                emit buttonPressed((InputDecoder::SNESButton)ev.code);
            Lpressed = !Lpressed;
        }
        if (ev.code == 5)
        {
            if (Rpressed)
                emit buttonReleased((InputDecoder::SNESButton)ev.code);
            else
                emit buttonPressed((InputDecoder::SNESButton)ev.code);
            Rpressed = !Rpressed;
        }
    }
}

void InputDecoder::decodeHexdump(QString toDecode)
{
    QString first = toDecode.left(32);
    QString second = toDecode.mid(32, 32);
    RawInputEvent   firstEvent = stringtoRawInputEvent(first);
    RawInputEvent   secondEvent = stringtoRawInputEvent(second);

    processEvent(firstEvent);
    processEvent(secondEvent);
    return ;
    /*int dumb = toDecode.indexOf("000100");
    qDebug() << dumb;
    QString pressReleased = toDecode.mid(dumb + 10, 2); //release/pressed
    qDebug() << pressReleased;
    QString inputStr = toDecode.mid(dumb + 6, 4); // input?
    qDebug() << inputStr;
    SNESButton     key;

    key = strToKey[inputStr];
    if (pressReleased == "01")
        emit buttonPressed(key);
    else
        emit buttonReleased(key);*/

}


// This match the snes classic structure size
struct mtimeval {
               quint32      tv_sec;
               quint32      tv_usec;
};


struct input_event {
    struct mtimeval time;
    quint16 type;
    quint16 code;
    qint32  value;
};

RawInputEvent    InputDecoder::binaryToRawInputEvent(QByteArray data)
{
    RawInputEvent toret;
    struct input_event* me = reinterpret_cast<struct input_event*>(data.data());

    toret.time_s = me->time.tv_sec;
    toret.time_us = me->time.tv_usec;
    toret.code = me->code;
    toret.type = me->type;
    toret.value = me->value;
    return toret;

}

void InputDecoder::decodeBinary(QByteArray toDecode)
{
    while (!toDecode.isEmpty())
    {
        processEvent(binaryToRawInputEvent(toDecode.left(16)));
        toDecode.remove(0, 16);
    }
}
