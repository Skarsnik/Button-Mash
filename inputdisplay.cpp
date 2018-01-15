#include "inputdisplay.h"
#include "ui_inputdisplay.h"
#include <QDomDocument>
#include <QFileInfo>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QDebug>

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
               this->setFixedSize(background.size().width() + 5, background.size().height() + 5);
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



    inputCo = new TelnetConnection("localhost", 1023, "root", "clover");
    controlCo = new TelnetConnection("localhost", 1023, "root", "clover");
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
    mapItems[mapButtonToText[button]]->show();
}

void InputDisplay::onButtonReleased(InputDecoder::SNESButton button)
{
    mapItems[mapButtonToText[button]]->hide();
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
