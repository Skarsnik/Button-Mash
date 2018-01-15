#ifndef INPUTDISPLAY_H
#define INPUTDISPLAY_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QWidget>

#include "../telnetconnection.h"
#include "../inputdecoder.h"

namespace Ui {
class InputDisplay;
}

class InputDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit InputDisplay(QString skin, QWidget *parent = 0);
    ~InputDisplay();

private slots:
    void    onInputConnected();
    void    onInputNewLine(QByteArray data);
    void    onButtonPressed(InputDecoder::SNESButton);
    void    onButtonReleased(InputDecoder::SNESButton);

private:
    Ui::InputDisplay *ui;

    QGraphicsScene* scene;
    QMap<QString, QGraphicsPixmapItem*> mapItems;
    QMap<InputDecoder::SNESButton, QString> mapButtonToText;
    void    closeEvent(QCloseEvent* ev);

    TelnetConnection*   inputCo;
    TelnetConnection*   controlCo;
    InputDecoder*       inputDecoder;
};

#endif // INPUTDISPLAY_H
