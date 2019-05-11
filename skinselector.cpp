#include <QDomDocument>
#include <QDebug>
#include <QFileDialog>

#include "skinselector.h"
#include "ui_skinselector.h"
#include "inputdisplay.h"
#include "skinparser.h"
#include "arduinocom.h"

#include <QDir>
#include <QStandardItemModel>

QSettings* globalSetting;

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
    globalSetting = new QSettings("skarsnik.nyo.fr", "InputDisplay");
    if (globalSetting->contains("skinFolder"))
    {
        setSkinPath(globalSetting->value("skinFolder").toString());
    } else {
        setSkinPath(qApp->applicationDirPath() + "/Skins");
    }
    timer.setInterval(50);
    timer.start();
    display = nullptr;
    //ui->statusLabel->setText("Trying to connect to the SNES Classic, be sure you have hakchi CE installed on it");
    connect(&timer, &QTimer::timeout, this, &SkinSelector::onTimerTimeout);
    inputSelector = new InputSourceSelector(this);
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

void    SkinSelector::restoreLastSkin()
{
    //Skin
    for (unsigned int i = 0; i < listModel->rowCount(); i++)
    {
        const RegularSkin& sk = listModel->item(i)->data(Qt::UserRole + 2).value<RegularSkin>();
        if (sk.file == globalSetting->value("lastSkin/regularSkinPath").toString())
        {
            ui->skinListView->setCurrentIndex(listModel->item(i)->index());
            on_skinListView_clicked(listModel->item(i)->index());
            currentSkin = sk;
            if (!sk.subSkins.isEmpty())
            {
                unsigned int j = 0;
                qDebug() << "Sub skin" << globalSetting->value("lastSkin/regularSubSkin").toString();
                foreach(RegularSkin ssk, sk.subSkins)
                {
                    qDebug() << ssk.name;
                    if (ssk.name == globalSetting->value("lastSkin/regularSubSkin").toString())
                    {
                        ui->subSkinListView->setCurrentIndex(subSkinModel->item(j)->index());
                        qDebug() << "Crash is here?";
                        currentSkin = ssk;
                        break;
                    }
                    j++;
                }
            }
            setPreviewScene(currentSkin);
            break;
        }
    }
    //Piano Display
    qDebug() << "Piano Display" << globalSetting->value("lastSkin/pianoDisplay");
    if (!globalSetting->value("lastSkin/pianoDisplay").toBool())
        return;
    ui->pianoCheckBox->setChecked(true);
    for (unsigned int i = 0; i < pianoModel->rowCount(); i++)
    {
        const PianoSkin& sk = pianoModel->item(i)->data(Qt::UserRole + 2).value<PianoSkin>();
        if (sk.file == globalSetting->value("lastSkin/pianoSkinPath").toString())
        {
            ui->pianoSkinListView->setCurrentIndex(pianoModel->item(i)->index());
            break;
        }
    }
}

void    SkinSelector::saveSkinStarted()
{
    globalSetting->setValue("lastSkin/pianoDisplay", ui->pianoCheckBox->isChecked());
    globalSetting->setValue("lastSkin/regularSkinPath", currentSkin.file);
    globalSetting->setValue("lastSkin/pianoSkinPath", pianoModel->itemFromIndex(
                             ui->pianoSkinListView->currentIndex())->data(Qt::UserRole + 2).value<PianoSkin>().file);
    globalSetting->setValue("lastSkin/regularSubSkin", currentSkin.name);

}

void    SkinSelector::setSkinPath(QString path)
{
    globalSetting->setValue("skinFolder", path);
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
            //qDebug() << skin;
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
            //qDebug() << pSkin;
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
    PianoSkin pSkin;
    if (ui->pianoCheckBox->isChecked())
    {
        pSkin = pianoModel->itemFromIndex(
                ui->pianoSkinListView->currentIndex())->data(Qt::UserRole + 2).value<PianoSkin>();
    }
    display = new InputDisplay(currentSkin, pSkin);
    display->setInputProvider(inputProvider);
    connect(display, &InputDisplay::closed, this, &SkinSelector::onDisplayClosed);
    display->show();
    inputProvider->start();
    this->hide();
    saveSkinStarted();
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
    else
        currentSkin = skin.subSkins.first();
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

void SkinSelector::onTimerTimeout()
{
    static bool first = true;
    if (first)
    {
        first = false;
        timer.setInterval(1000);
        restoreLastSkin();
        inputProvider = inputSelector->getLastProvider();
        if (inputProvider != nullptr)
            ui->sourceLabel->setText(QString("<b>%1</b>").arg(inputProvider->name()));
        else
            ui->sourceLabel->setText(tr("No Source provider selected"));
        return ;
    }
    if (display != nullptr && display->isVisible())
        return ;
    ui->startButton->setEnabled(inputProvider->isReady());
    ui->statusLabel->setText(inputProvider->statusText());
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


void SkinSelector::onDisplayClosed()
{
    show();
    inputProvider->stop();
}

void SkinSelector::on_changeSourceButton_clicked()
{
    if (inputSelector->exec())
    {
        inputProvider = inputSelector->currentProvider();
        ui->sourceLabel->setText(QString("<b>%1</b>").arg(inputProvider->name()));
    }
}


void SkinSelector::on_configHSButton_clicked()
{
    static bool hide = true;
    return ;

    if (hide)
        ui->configFrame->hide();
    else
        ui->configFrame->show();
    hide = !hide;
}
