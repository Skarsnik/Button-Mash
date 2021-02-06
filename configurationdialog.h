#ifndef CONFIGURATIONDIALOG_H
#define CONFIGURATIONDIALOG_H

#include <QDialog>

namespace Ui {
class ConfigurationDialog;
}

class ConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigurationDialog(QWidget *parent = nullptr);
    ~ConfigurationDialog();

    int delay;

private slots:
    void on_delayRadioButton_clicked(bool checked);

private:
    Ui::ConfigurationDialog *ui;

    // QDialog interface
public slots:
    void accept();
};

#endif // CONFIGURATIONDIALOG_H
