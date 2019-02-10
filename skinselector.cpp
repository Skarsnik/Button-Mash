#include <QDomDocument>
#include <QDebug>
#include <QFileDialog>

#include "skinselector.h"
#include "ui_skinselector.h"
#include "inputdisplay.h"

#define SNES_CLASSIC_IP "169.254.13.37"

#include <QDir>
#include <QStandardItemModel>

SkinSelector::SkinSelector(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SkinSelector)
{
    ui->setupUi(this);
    listModel = new QStandardItemModel();
    ui->skinListView->setModel(listModel);
    pianoModel = new QStandardItemModel();
    ui->pianoSkinListView->setModel(pianoModel);
    m_settings = new QSettings("skarsnik.nyo.fr", "InputDisplay");
    if (m_settings->contains("skinFolder"))
    {
        setSkinPath(m_settings->value("skinFolder").toString());
    } else {
        setSkinPath(qApp->applicationDirPath() + "/Skins");
    }
    testCo = new TelnetConnection(SNES_CLASSIC_IP, 23, "root", "clover");
    testCo->conneect();
    timer.setInterval(1000);
    timer.start();
    display = nullptr;
    ui->statusLabel->setText("Trying to connect to the SNES Classic, be sure you have hakchi CE installed on it");
    connect(&timer, &QTimer::timeout, this, &SkinSelector::onTimerTimeout);
    connect(testCo, &TelnetConnection::connected, this, &SkinSelector::onTelnetConnected);
}

void    SkinSelector::addToList(QStandardItemModel* model, QString xmlPath)
{
    QFile skinFile(xmlPath);
    skinFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString xmlSkin = skinFile.readAll();
    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorColumn;
    if (doc.setContent(xmlSkin, true, &errorStr, &errorLine,
                                   &errorColumn))
    {
        qDebug() << "Added " << doc.firstChildElement().attribute("name");
        QStandardItem* item = new QStandardItem(QString("%1 by %2").arg(
                    doc.firstChildElement().attribute("name")).arg(
                    doc.firstChildElement().attribute("author")));
        item->setData(xmlPath, Qt::UserRole + 2);
        model->appendRow(item);
    }
}

void    SkinSelector::setPreviewScene(QString skin)
{
    QString errorStr;
    int errorLine;
    int errorColumn;
    QDomDocument doc;
    QGraphicsScene* scene = ui->previewGraphicView->scene();
    if (scene == nullptr)
    {
        scene = new QGraphicsScene();
        ui->previewGraphicView->setScene(scene);
    }
    scene->clear();
    QFileInfo fi(skin);
    qDebug() << skin << QFileInfo::exists(skin);
    QFile   skinFile(skin);
    skinFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString xmlSkin = skinFile.readAll();
    if (!doc.setContent(xmlSkin, true, &errorStr, &errorLine,
                                   &errorColumn)) {
        return;
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
               //newPix->hide();
           }
           child = child.nextSiblingElement();
   }
   ui->previewGraphicView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatioByExpanding);
}

void    SkinSelector::setSkinPath(QString path)
{
    m_settings->setValue("skinFolder", path);
    ui->skinPathEdit->setText(path);
    QDir dir(path);
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    auto list = dir.entryInfoList();
    foreach(QFileInfo fi, list)
    {
        qDebug() << fi.fileName();
        if (QFileInfo::exists(fi.absoluteFilePath() + "/skin.xml"))
        {
            qDebug() << "Found a skin file";
            addToList(listModel, fi.absoluteFilePath() + "/skin.xml");
        }
        if (QFileInfo::exists(fi.absoluteFilePath() + "/pianodisplay.xml"))
        {
            qDebug() << "Found a piano skin file";
            addToList(pianoModel, fi.absoluteFilePath() + "/pianodisplay.xml");
        }

    }
}

SkinSelector::~SkinSelector()
{
    delete ui;
}



void SkinSelector::on_startButton_clicked()
{
    testCo->close();
    QString skinPath = listModel->itemFromIndex(
                    ui->skinListView->currentIndex())->data(Qt::UserRole + 2).toString();
    QString pianoPath;
    if (ui->pianoCheckBox->isChecked())
        pianoPath = pianoModel->itemFromIndex(
                ui->pianoSkinListView->currentIndex())->data(Qt::UserRole + 2).toString();
    display = new InputDisplay(skinPath, pianoPath);
    connect(display, &InputDisplay::closed, this, &SkinSelector::show);
    display->show();
    this->hide();
}

void SkinSelector::on_pianoCheckBox_stateChanged(int arg1)
{
    ui->pianoSkinListView->setEnabled(arg1 != 0);
}

void SkinSelector::on_skinListView_clicked(const QModelIndex &index)
{
    setPreviewScene(listModel->itemFromIndex(index)->data(Qt::UserRole + 2).toString());
}

void SkinSelector::onTelnetConnected()
{
    ui->statusLabel->setText("Connection etablished, ready to go");
    ui->startButton->setEnabled(true);
}

void SkinSelector::onTimerTimeout()
{
    if (display != nullptr && display->isVisible())
        return ;
    if (testCo->state() != TelnetConnection::Connected)
    {
        ui->statusLabel->setText("Trying to connect to the SNES Classic, be sure you have hakchi CE installed on it");
        ui->startButton->setEnabled(false);
        testCo->conneect();
    } else {
        ui->startButton->setEnabled(true);
    }
}

void SkinSelector::on_skinPathButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose the default folder for skins"), qApp->applicationDirPath() + "/Skins");
    setSkinPath(dir);
}
