#include "arduinocom.h"
#include "inputdisplay.h"
#include "skinparser.h"
#include "ui_inputdisplay.h"
#include <QDomDocument>
#include <QFileInfo>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QDebug>

#define SNES_CLASSIC_IP "169.254.13.37"

InputDisplay::InputDisplay(RegularSkin skin, PianoSkin pSkin, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InputDisplay)
{
    ui->setupUi(this);
    QDomDocument    doc;
    scene = new QGraphicsScene();
    QFileInfo fi(skin.file);
    QPixmap background(fi.absolutePath() + "/" + skin.background);
    scene->setSceneRect(0, 0, background.size().width(), background.size().height());
    //this->setFixedSize(background.size().width() + 5, background.size().height() + 5);
    scene->addPixmap(background);
    foreach(RegularButtonSkin but, skin.buttons)
    {
        QPixmap pix(fi.absolutePath() + "/" + but.image);
        QGraphicsPixmapItem* newPix = new QGraphicsPixmapItem(pix.scaled(but.width, but.height));
        newPix->setPos(but.x, but.y);
        newPix->setZValue(1);
        scene->addItem(newPix);
        mapItems[but.name] = newPix;
        newPix->hide();
    }
    ui->graphicsView->setScene(scene);

    /*connect(arduino, &ArduinoCOM::buttonPressed, this, &InputDisplay::onButtonPressed);
    connect(arduino, &ArduinoCOM::buttonReleased, this, &InputDisplay::onButtonReleased);*/

    mapButtonToText[InputProvider::SNESButton::A] = "a";
    mapButtonToText[InputProvider::SNESButton::B] = "b";
    mapButtonToText[InputProvider::SNESButton::X] = "x";
    mapButtonToText[InputProvider::SNESButton::Y] = "y";
    mapButtonToText[InputProvider::SNESButton::L] = "l";
    mapButtonToText[InputProvider::SNESButton::R] = "r";
    mapButtonToText[InputProvider::SNESButton::Start] = "start";
    mapButtonToText[InputProvider::SNESButton::Select] = "select";
    mapButtonToText[InputProvider::SNESButton::Up] = "up";
    mapButtonToText[InputProvider::SNESButton::Down] = "down";
    mapButtonToText[InputProvider::SNESButton::Right] = "right";
    mapButtonToText[InputProvider::SNESButton::Left] = "left";

    pianoTimer.setInterval(33);
    pianoTimer.start();
    pianoTimeRange = 3000;
    pianoHeight = 0;
    ui->pianoLabel->setVisible(false);
    ui->pianoTagLabel->setVisible(false);
    int windowWidth = scene->sceneRect().width() + 10;
    if (!pSkin.name.isEmpty())
    {
        configPianoDisplay(pSkin);
        ui->pianoLabel->setPixmap(*pianoDisplay);
        setPianoLabel();
        if (pianoDisplay->width() > windowWidth)
            windowWidth = pianoDisplay->width();
        pianoHeight = pianoDisplay->height();
        connect(&pianoTimer, SIGNAL(timeout()), this, SLOT(onPianoTimerTimeout()));
        ui->pianoLabel->setVisible(true);
        ui->pianoTagLabel->setVisible(true);
    }
    this->setFixedSize(windowWidth, scene->sceneRect().height() + pianoHeight + 45);
    this->setStyleSheet("background-color: black;");
}

void InputDisplay::setInputProvider(InputProvider *inp)
{
    inputProvider = inp;
    connect(inputProvider, &InputProvider::buttonPressed, this, &InputDisplay::onButtonPressed);
    connect(inputProvider, &InputProvider::buttonReleased, this, &InputDisplay::onButtonReleased);

}

void    InputDisplay::configPianoDisplay(PianoSkin pSkin)
{
    QFileInfo fi(pSkin.file);
    pianoDisplay = new QPixmap(pSkin.width, pSkin.height);
    foreach(PianoButton pBut, pSkin.buttons)
    {
        InputProvider::SNESButton but = mapButtonToText.key(pBut.name);
        pianoButPos[but] = pBut.x;
        pianoButColor[but] = pBut.color;
        pianoButWidth[but] = pBut.width;
    }
}

void    InputDisplay::setPianoLabel()
{
    QPixmap*    tag = new QPixmap(400, 30);
    QMap<InputProvider::SNESButton, QString>        alternateText;
    tag->fill(Qt::black);
    QPainter painter(tag);
    QFont font("DejaVu Sans Mono");
    font.setBold(true);
    font.setPixelSize(12);
    painter.setFont(font);

    alternateText[InputProvider::Left] = "<";
    alternateText[InputProvider::Right] = ">";
    alternateText[InputProvider::Up] = "^";
    alternateText[InputProvider::Down] = "v";
    alternateText[InputProvider::Start] = "ST";
    alternateText[InputProvider::Select] = "SEL";

    foreach(InputProvider::SNESButton but, mapButtonToText.keys())
    {
        QString butText;
        if (alternateText.contains(but))
            butText = alternateText[but];
        else
            butText = mapButtonToText[but].toUpper();
        painter.setPen(pianoButColor[but]);
        painter.drawText(QRect(pianoButPos[but] - 5, 0, 30, 20), Qt::AlignCenter, butText);
    }
    ui->pianoTagLabel->setPixmap(*tag);
}


void InputDisplay::onButtonPressed(InputProvider::SNESButton button)
{
    if (mapItems.contains(mapButtonToText[button]))
        mapItems[mapButtonToText[button]]->show();
    PianoEvent pe;
    pe.startTime = QTime::currentTime();
    pianoEvents[button].append(pe);
}

void InputDisplay::onButtonReleased(InputProvider::SNESButton button)
{
    if (mapItems.contains(mapButtonToText[button]))
        mapItems[mapButtonToText[button]]->hide();
    pianoEvents[button].last().endTime = QTime::currentTime();
}

void    InputDisplay::filterPianoEvent()
{
        QTime now = QTime::currentTime();
        QTime bottomTime = now.addMSecs(static_cast<int>(pianoTimeRange) * -1);
        foreach (InputProvider::SNESButton but, pianoEvents.keys())
        {
            QMutableListIterator<PianoEvent> iPe(pianoEvents[but]);
            while (iPe.hasNext())
            {
                PianoEvent pe = iPe.next();
                if (!pe.endTime.isNull() && pe.endTime < bottomTime)
                    iPe.remove();

            }
        }
}

void InputDisplay::onPianoTimerTimeout()
{
    QTime now = QTime::currentTime();
    QTime bottomTime = now.addMSecs(static_cast<int>(pianoTimeRange) * -1);
    filterPianoEvent();
    QPainter pa(pianoDisplay);
    pa.fillRect(pianoDisplay->rect(), Qt::black);

    foreach (InputProvider::SNESButton but, pianoEvents.keys())
    {
        const QList<PianoEvent> ev = pianoEvents[but];
            foreach(PianoEvent pe, ev)
            {
                //qDebug() << pe.startTime << pe.endTime;
                int yRect, hRect;
                if (pe.endTime.isNull())
                {
                    yRect = 0;
                    hRect = (pe.startTime.msecsTo(now) * pianoHeight) / pianoTimeRange;
                }
                else
                {
                    yRect = (pe.endTime.msecsTo(now) * pianoHeight) / pianoTimeRange;
                    hRect = (pe.startTime.msecsTo(pe.endTime) * pianoHeight) / pianoTimeRange;
                }
                QRect   rect(pianoButPos[but], yRect, pianoButWidth[but], hRect);
                //qDebug() << rect;

                //pa.setPen(Qt::blue);
                pa.fillRect(rect, pianoButColor[but]);
            }

    }
    ui->pianoLabel->setPixmap(*pianoDisplay);
    ui->pianoLabel->update();
}

void InputDisplay::closeEvent(QCloseEvent *ev)
{
     emit closed();
}


InputDisplay::~InputDisplay()
{
    delete ui;
}
