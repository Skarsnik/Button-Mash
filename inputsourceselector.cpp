#include "inputsourceselector.h"
#include "ui_inputsourceselector.h"

InputSourceSelector::InputSourceSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InputSourceSelector)
{
    ui->setupUi(this);
}

InputSourceSelector::~InputSourceSelector()
{
    delete ui;
}
