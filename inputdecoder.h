
#ifndef INPUTDECODER_H
#define INPUTDECODER_H

#include <QObject>
#include <QMap>
#include <inputprovider.h>

struct RawInputEvent
{
    quint32 time_s;
    quint32 time_us;
    quint16 type;
    quint16 code;
    qint32 value;
};


class InputDecoder : public InputProvider
{
    Q_OBJECT
public:

    InputDecoder();
public slots:
    void    decodeHexdump(QString toDecode);
    void    decodeBinary(QByteArray toDecode);

private:
    void processEvent(RawInputEvent ev);
    bool    Lpressed;
    bool    Rpressed;
    RawInputEvent binaryToRawInputEvent(QByteArray data);
};

#endif // INPUTDECODER_H
