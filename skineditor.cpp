#include <QFileDialog>
#include <QGraphicsPixmapItem>
#include <QSettings>
#include "skineditor.h"
#include "ui_skineditor.h"

extern QSettings* globalSetting;

SkinEditor::SkinEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SkinEditor)
{
    ui->setupUi(this);
    scene = new QGraphicsScene();
    ui->skinView->setScene(scene);
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
    buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(ui->AButton, InputProvider::SNESButton::A);
    buttonGroup->addButton(ui->BButton, InputProvider::SNESButton::B);
    buttonGroup->addButton(ui->XButton, InputProvider::SNESButton::X);
    buttonGroup->addButton(ui->YButton, InputProvider::SNESButton::Y);
    buttonGroup->addButton(ui->LButton, InputProvider::SNESButton::L);
    buttonGroup->addButton(ui->RButton, InputProvider::SNESButton::R);
    buttonGroup->addButton(ui->startButton, InputProvider::SNESButton::Start);
    buttonGroup->addButton(ui->selectButton, InputProvider::SNESButton::Select);
    buttonGroup->addButton(ui->upButton, InputProvider::SNESButton::Up);
    buttonGroup->addButton(ui->leftButton, InputProvider::SNESButton::Left);
    buttonGroup->addButton(ui->downButton, InputProvider::SNESButton::Down);
    buttonGroup->addButton(ui->rightButton, InputProvider::SNESButton::Right);
    connect(buttonGroup, qOverload<QAbstractButton*>(&QButtonGroup::buttonClicked), this, &SkinEditor::onSnesButtonClicked);
    defaultSkinPath = globalSetting->value("skinFolder").toString();
}

SkinEditor::~SkinEditor()
{
    delete ui;
}

void SkinEditor::on_openDir_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Select the Skin file", defaultSkinPath, "XML files (*.xml)");
    if (!file.isEmpty())
    {
        skin = SkinParser::parseRegularSkin(file);
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
            mapItems[but.name] = newPix;
            newPix->hide();
        }
    }
}

void SkinEditor::onSnesButtonClicked(QAbstractButton* but)
{
    int id = buttonGroup->id(but);
    setCurrentButton(id);
}

void SkinEditor::setCurrentButton(int id)
{
    QString buttonString = mapButtonToText[static_cast<InputProvider::SNESButton>(id)];
    if (mapItems.contains(buttonString))
    {
        ui->addNewButton->setEnabled(false);
        auto pix = mapItems[buttonString];
        pix->setVisible(true);
        ui->xSpinBox->setEnabled(true);
        ui->ySpinBox->setEnabled(true);
        ui->widthSpinBox->setEnabled(true);
        ui->heightSpinBox->setEnabled(true);
        ui->xSpinBox->setValue(pix->x());
        ui->ySpinBox->setValue(pix->y());
        ui->widthSpinBox->setValue(pix->boundingRect().width());
        ui->heightSpinBox->setValue(pix->boundingRect().height());
    } else {
        ui->addNewButton->setEnabled(true);
        ui->xSpinBox->setEnabled(false);
        ui->ySpinBox->setEnabled(false);
        ui->widthSpinBox->setEnabled(false);
        ui->heightSpinBox->setEnabled(false);
        ui->xSpinBox->setValue(0);
        ui->ySpinBox->setValue(0);
        ui->widthSpinBox->setValue(0);
        ui->heightSpinBox->setValue(0);
    }
}


void SkinEditor::on_addButtonButton_clicked()
{

}

