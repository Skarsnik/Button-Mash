#ifndef INPUTDISPLAY_H
#define INPUTDISPLAY_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QTime>
#include <QTimer>
#include <QWidget>

#include "skinparser.h"
#include "../telnetconnection.h"
#include "../inputdecoder.h"

namespace Ui {
class InputDisplay;
}

class InputDisplay : public QWidget
{
    Q_OBJECT

    struct PianoEvent {
        QTime   startTime;
        QTime   endTime;
    };

public:
    explicit InputDisplay(RegularSkin skin, PianoSkin pSkin, QWidget *parent = nullptr);
    ~InputDisplay();

signals:
    void    closed();

private slots:
    void    onInputConnected();
    void    onInputNewLine(QByteArray data);
    void    onButtonPressed(InputDecoder::SNESButton);
    void    onButtonReleased(InputDecoder::SNESButton);
    void    onPianoTimerTimeout();

private:
    Ui::InputDisplay *ui;

    QGraphicsScene* scene;
    QMap<QString, QGraphicsPixmapItem*> mapItems;
    QMap<InputDecoder::SNESButton, QString> mapButtonToText;
    QMap<InputDecoder::SNESButton, QList<PianoEvent> >  pianoEvents;
    QMap<InputDecoder::SNESButton, uint>                pianoButPos;
    QMap<InputDecoder::SNESButton, QColor>              pianoButColor;
    QMap<InputDecoder::SNESButton, uint>                pianoButWidth;
    void    closeEvent(QCloseEvent* ev);

    TelnetConnection*   inputCo;
    TelnetConnection*   controlCo;
    InputDecoder*       inputDecoder;
    QPixmap*            pianoDisplay;
    QTimer              pianoTimer;
    uint                pianoHeight;
    uint                pianoTimeRange;
    void filterPianoEvent();
    void setPianoLabel();
    void configPianoDisplay(PianoSkin pSkin);

};

#endif // INPUTDISPLAY_H
