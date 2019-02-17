#ifndef INPUTPROVIDER_H
#define INPUTPROVIDER_H

#include <QObject>

enum SNESButton
{
    Up = 0x2C2,
    Down = 0x2C3,
    Left = 0x2C0,
    Right = 0x2C1,
    X = 0x133,
    A = 0x130,
    B = 0x131,
    Y = 0x134,
    L = 2,
    R = 5,
    Start = 0x13B,
    Select = 0x13A
};

class InputProvider : public QObject
{
    Q_OBJECT
public:
    explicit InputProvider(QObject *parent = nullptr);

signals:
    void    buttonPressed(SNESButton but);
    void    buttonReleased(SNESButton but);

public slots:
};

#endif // INPUTPROVIDER_H
