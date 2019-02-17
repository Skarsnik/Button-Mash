#ifndef INPUTDISPLAY_H
#define INPUTDISPLAY_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QTime>
#include <QTimer>
#include <QWidget>

#include "skinparser.h"
#include "inputprovider.h"

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
    void     setInputProvider(InputProvider* inp);
    ~InputDisplay();

signals:
    void    closed();

private slots:
    void    onButtonPressed(InputProvider::SNESButton);
    void    onButtonReleased(InputProvider::SNESButton);
    void    onPianoTimerTimeout();

private:
    Ui::InputDisplay *ui;

    QGraphicsScene* scene;
    QMap<QString, QGraphicsPixmapItem*> mapItems;
    QMap<InputProvider::SNESButton, QString> mapButtonToText;
    QMap<InputProvider::SNESButton, QList<PianoEvent> >  pianoEvents;
    QMap<InputProvider::SNESButton, uint>                pianoButPos;
    QMap<InputProvider::SNESButton, QColor>              pianoButColor;
    QMap<InputProvider::SNESButton, uint>                pianoButWidth;
    void    closeEvent(QCloseEvent* ev);

    InputProvider*      inputProvider;
    QPixmap*            pianoDisplay;
    QTimer              pianoTimer;
    uint                pianoHeight;
    uint                pianoTimeRange;
    void filterPianoEvent();
    void setPianoLabel();
    void configPianoDisplay(PianoSkin pSkin);

};

#endif // INPUTDISPLAY_H
