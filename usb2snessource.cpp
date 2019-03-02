#include "usb2snessource.h"

static const unsigned int defaultAddress = 0xF90718;

Usb2SnesSource::Usb2SnesSource(USB2snes* usb)
{
    address = defaultAddress;
    timer.setInterval(10);
    usb2snes = usb;
}

void Usb2SnesSource::start()
{
    timer.start();
}

void Usb2SnesSource::onTimerTick()
{
    static quint16 prev_input = 0;
    QByteArray input = usb2snes->getAddress(address, 2);
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
