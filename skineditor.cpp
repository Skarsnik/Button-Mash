#include "skineditor.h"
#include "ui_skineditor.h"

SkinEditor::SkinEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SkinEditor)
{
    ui->setupUi(this);
}

SkinEditor::~SkinEditor()
{
    delete ui;
}
