#include "inputdisplay.h"
#include "ui_inputdisplay.h"
#include <QDomDocument>
#include <QFileInfo>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QDebug>

#define SNES_CLASSIC_IP "169.254.13.37"

InputDisplay::InputDisplay(QString skin, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InputDisplay)
{
    ui->setupUi(this);
    QDomDocument    doc;
    scene = new QGraphicsScene();
    QFileInfo fi(skin);
    QString errorStr;
    int errorLine;
    int errorColumn;
    qDebug() << skin << QFileInfo::exists(skin);
    QFile   skinFile(skin);
    skinFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString xmlSkin = skinFile.readAll();
    if (!doc.setContent(xmlSkin, true, &errorStr, &errorLine,
                                   &errorColumn)) {
    QMessageBox::information(window(), tr("DOM Bookmarks"),
                                    tr("Parse error at line %1, column %2:\n%3")
                                    .arg(errorLine)
                                    .arg(errorColumn)
                                    .arg(errorStr));
    }
    QDomElement root = doc.documentElement();
    QDomElement child = root.firstChildElement();
    while (!child.isNull()) {
           if (child.tagName() == "background")
           {
               qDebug() << fi.absolutePath() + "/" + child.attribute("image");
               QPixmap background(fi.absolutePath() + "/" + child.attribute("image"));
               scene->setSceneRect(0, 0, background.size().width(), background.size().height());
               //this->setFixedSize(background.size().width() + 5, background.size().height() + 5);
               scene->addPixmap(background);
           }
           if (child.tagName() == "button")
           {
               QPixmap pix(fi.absolutePath() + "/" + child.attribute("image"));
               unsigned int width = child.attribute("width").toUInt();
               unsigned int height = child.attribute("height").toUInt();
               QGraphicsPixmapItem* newPix = new QGraphicsPixmapItem(pix.scaled(width, height));
               newPix->setPos(child.attribute("x").toInt(), child.attribute("y").toInt());
               newPix->setZValue(1);
               scene->addItem(newPix);
               mapItems[child.attribute("name")] = newPix;
               newPix->hide();
           }
           child = child.nextSiblingElement();
   }
    ui->graphicsView->setScene(scene);



    inputCo = new TelnetConnection(SNES_CLASSIC_IP, 23, "root", "clover");
    controlCo = new TelnetConnection(SNES_CLASSIC_IP, 23, "root", "clover");
    inputDecoder = new InputDecoder();
    inputCo->debugName = "Input";
    inputCo->conneect();
    controlCo->conneect();
    inputCo->setOneCommandMode(true);
    connect(inputCo, SIGNAL(commandReturnedNewLine(QByteArray)), this, SLOT(onInputNewLine(QByteArray)));
    connect(inputCo, SIGNAL(connected()), this, SLOT(onInputConnected()));
    connect(inputDecoder, SIGNAL(buttonPressed(InputDecoder::SNESButton)), this, SLOT(onButtonPressed(InputDecoder::SNESButton)));
    connect(inputDecoder, SIGNAL(buttonReleased(InputDecoder::SNESButton)), this, SLOT(onButtonReleased(InputDecoder::SNESButton)));

    mapButtonToText[InputDecoder::SNESButton::A] = "a";
    mapButtonToText[InputDecoder::SNESButton::B] = "b";
    mapButtonToText[InputDecoder::SNESButton::X] = "x";
    mapButtonToText[InputDecoder::SNESButton::Y] = "y";
    mapButtonToText[InputDecoder::SNESButton::L] = "l";
    mapButtonToText[InputDecoder::SNESButton::R] = "r";
    mapButtonToText[InputDecoder::SNESButton::Start] = "start";
    mapButtonToText[InputDecoder::SNESButton::Select] = "select";
    mapButtonToText[InputDecoder::SNESButton::Up] = "up";
    mapButtonToText[InputDecoder::SNESButton::Down] = "down";
    mapButtonToText[InputDecoder::SNESButton::Right] = "right";
    mapButtonToText[InputDecoder::SNESButton::Left] = "left";

    pianoHeight = 200;
    pianoTimer.setInterval(33);
    pianoTimer.start();
    pianoTimeRange = 3000;
    connect(&pianoTimer, SIGNAL(timeout()), this, SLOT(onPianoTimerTimeout()));
    this->setFixedSize(405, scene->sceneRect().height() + pianoHeight + 45);
    pianoDisplay = new QPixmap(400, pianoHeight);
    pianoDisplay->fill(Qt::black);
    ui->pianoLabel->setPixmap(*pianoDisplay);
    pianoButPos[InputDecoder::SNESButton::Left] = 10;
    pianoButPos[InputDecoder::SNESButton::Up] = 40;
    pianoButPos[InputDecoder::SNESButton::Right] = 70;
    pianoButPos[InputDecoder::SNESButton::Down] = 100;
    pianoButPos[InputDecoder::SNESButton::Select] = 130;
    pianoButPos[InputDecoder::SNESButton::Start] = 160;
    pianoButPos[InputDecoder::SNESButton::A] = 280;
    pianoButPos[InputDecoder::SNESButton::B] = 220;
    pianoButPos[InputDecoder::SNESButton::Y] = 190;
    pianoButPos[InputDecoder::SNESButton::X] = 250;
    pianoButPos[InputDecoder::SNESButton::L] = 310;
    pianoButPos[InputDecoder::SNESButton::R] = 340;

    pianoButColor[InputDecoder::SNESButton::Left] = Qt::gray;
    pianoButColor[InputDecoder::SNESButton::Up] = Qt::gray;
    pianoButColor[InputDecoder::SNESButton::Right] = Qt::gray;
    pianoButColor[InputDecoder::SNESButton::Down] = Qt::gray;
    pianoButColor[InputDecoder::SNESButton::Start] = Qt::lightGray;
    pianoButColor[InputDecoder::SNESButton::Select] = Qt::lightGray;
    pianoButColor[InputDecoder::SNESButton::A] = Qt::red;
    pianoButColor[InputDecoder::SNESButton::B] = Qt::yellow;
    pianoButColor[InputDecoder::SNESButton::Y] = Qt::green;
    pianoButColor[InputDecoder::SNESButton::X] = Qt::blue;
    pianoButColor[InputDecoder::SNESButton::L] = Qt::gray;
    pianoButColor[InputDecoder::SNESButton::R] = Qt::gray;
    setPianoLabel();
}

void    InputDisplay::setPianoLabel()
{
    QPixmap*    tag = new QPixmap(400, 30);
    QMap<InputDecoder::SNESButton, QString>        alternateText;
    tag->fill(Qt::black);
    QPainter painter(tag);
    QFont font("DejaVu Sans Mono");
    font.setBold(true);
    font.setPixelSize(12);
    painter.setFont(font);

    alternateText[InputDecoder::Left] = "<";
    alternateText[InputDecoder::Right] = ">";
    alternateText[InputDecoder::Up] = "^";
    alternateText[InputDecoder::Down] = "v";
    alternateText[InputDecoder::Start] = "ST";
    alternateText[InputDecoder::Select] = "SEL";

    foreach(InputDecoder::SNESButton but, mapButtonToText.keys())
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


void InputDisplay::onInputConnected()
{
    inputCo->executeCommand("hexdump -v -e '32/1 \"%02X\" \"\\n\"' /dev/input/by-path/platform-twi.1-event-joystick");
}

void InputDisplay::onInputNewLine(QByteArray data)
{
    inputDecoder->decodeHexdump(data);
}

void InputDisplay::onButtonPressed(InputDecoder::SNESButton button)
{
    if (mapItems.contains(mapButtonToText[button]))
        mapItems[mapButtonToText[button]]->show();
    PianoEvent pe;
    pe.startTime = QTime::currentTime();
    pianoEvents[button].append(pe);
}

void InputDisplay::onButtonReleased(InputDecoder::SNESButton button)
{
    if (mapItems.contains(mapButtonToText[button]))
        mapItems[mapButtonToText[button]]->hide();
    pianoEvents[button].last().endTime = QTime::currentTime();
}

void    InputDisplay::filterPianoEvent()
{
        QTime now = QTime::currentTime();
        QTime bottomTime = now.addMSecs(static_cast<int>(pianoTimeRange) * -1);
        foreach (InputDecoder::SNESButton but, pianoEvents.keys())
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

    foreach (InputDecoder::SNESButton but, pianoEvents.keys())
    {
        const QList<PianoEvent> ev = pianoEvents[but];
            foreach(PianoEvent pe, ev)
            {
                qDebug() << pe.startTime << pe.endTime;
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
                QRect   rect(pianoButPos[but], yRect, 20, hRect);
                qDebug() << rect;

                //pa.setPen(Qt::blue);
                pa.fillRect(rect, pianoButColor[but]);
            }

    }
    ui->pianoLabel->setPixmap(*pianoDisplay);
    ui->pianoLabel->update();
}

void InputDisplay::closeEvent(QCloseEvent *ev)
{
    controlCo->syncExecuteCommand("killall hexdump");
    inputCo->close();
    controlCo->close();
}


InputDisplay::~InputDisplay()
{
    delete ui;
}
