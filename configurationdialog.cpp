#include "configurationdialog.h"
#include "ui_configurationdialog.h"

ConfigurationDialog::ConfigurationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigurationDialog)
{
    ui->setupUi(this);
    delay = 0;
}

ConfigurationDialog::~ConfigurationDialog()
{
    delete ui;
}

void ConfigurationDialog::on_delayRadioButton_clicked(bool checked)
{
    ui->delaySpinBox->setEnabled(checked);
}


void ConfigurationDialog::accept()
{
    delay = ui->delaySpinBox->value();
}
