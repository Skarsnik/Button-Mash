#include <QDomDocument>
#include <QDebug>
#include <QFileDialog>

#include "skinselector.h"
#include "ui_skinselector.h"
#include "inputdisplay.h"
#include "skinparser.h"

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
    subSkinModel = new QStandardItemModel();
    ui->subSkinListView->setModel(subSkinModel);
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

void    SkinSelector::setPreviewScene(const RegularSkin& skin)
{
    QGraphicsScene* scene = ui->previewGraphicView->scene();
    if (scene == nullptr)
    {
        scene = new QGraphicsScene();
        ui->previewGraphicView->setScene(scene);
    }
    scene->clear();
    QFileInfo fi(skin.file);
    QPixmap background(fi.absolutePath() + "/" + skin.background);
    scene->setSceneRect(0, 0, background.size().width(), background.size().height());
    scene->addPixmap(background);
    foreach(RegularButtonSkin but, skin.buttons)
    {
        QPixmap pix(fi.absolutePath() + "/" + but.image);
        QGraphicsPixmapItem* newPix = new QGraphicsPixmapItem(pix.scaled(but.width, but.height));
        newPix->setPos(but.x, but.y);
        newPix->setZValue(1);
        scene->addItem(newPix);
    }
    ui->previewGraphicView->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
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
            RegularSkin skin = SkinParser::parseRegularSkin(fi.absoluteFilePath() + "/skin.xml");
            if (!SkinParser::xmlError.isEmpty())
                qDebug() << SkinParser::xmlError;
            qDebug() << skin;
            QStandardItem* item = new QStandardItem(QString(tr("%1 by %2")).arg(skin.name).arg(skin.author));
            QVariant var;
            var.setValue(skin);
            item->setData(var, Qt::UserRole + 2);
            listModel->appendRow(item);
        }
        if (QFileInfo::exists(fi.absoluteFilePath() + "/pianodisplay.xml"))
        {
            qDebug() << "Found a piano skin file";
            PianoSkin pSkin = SkinParser::parsePianoSkin(fi.absoluteFilePath() + "/pianodisplay.xml");
            qDebug() << pSkin;
            QStandardItem* item = new QStandardItem(QString(tr("%1 by %2")).arg(pSkin.name).arg(pSkin.author));
            QVariant var;
            var.setValue(pSkin);
            item->setData(var, Qt::UserRole + 2);
            pianoModel->appendRow(item);
            ui->pianoSkinListView->setCurrentIndex(pianoModel->item(0)->index());
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
    PianoSkin pSkin;
    if (ui->pianoCheckBox->isChecked())
    {
        pSkin = pianoModel->itemFromIndex(
                ui->pianoSkinListView->currentIndex())->data(Qt::UserRole + 2).value<PianoSkin>();
    }
    display = new InputDisplay(currentSkin, pSkin);
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
    const RegularSkin& skin = listModel->itemFromIndex(index)->data(Qt::UserRole + 2).value<RegularSkin>();
    subSkinModel->clear();
    if (skin.subSkins.isEmpty())
        currentSkin = skin;
    foreach(RegularSkin sk, skin.subSkins)
    {
        QStandardItem* item = new QStandardItem(sk.name);
        QVariant var;
        var.setValue(sk);
        item->setData(var, Qt::UserRole + 2);
        subSkinModel->appendRow(item);
        ui->subSkinListView->setCurrentIndex(subSkinModel->item(0)->index());
    }
    setPreviewScene(skin);
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

void SkinSelector::on_subSkinListView_clicked(const QModelIndex &index)
{
    const RegularSkin& skin = subSkinModel->itemFromIndex(index)->data(Qt::UserRole + 2).value<RegularSkin>();
    currentSkin = skin;
    setPreviewScene(skin);
}
