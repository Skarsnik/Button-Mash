#ifndef MAPBUTTONDIALOG_H
#define MAPBUTTONDIALOG_H

#include "inputprovider.h"

#include <QDialog>
#include <QGamepadManager>

namespace Ui {
class MapButtonDialog;
}

class MapButtonDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MapButtonDialog(QWidget *parent = nullptr);
    ~MapButtonDialog();
    struct QGamepadInputInfos {
        QGamepadManager::GamepadButton  button;
        QGamepadManager::GamepadAxis    axis;
        double                          value;
    };

private slots:
    void    on_upButton_clicked();
    void    onGamepadButtonPressEvent(int deviceId, QGamepadManager::GamepadButton button, double value);
    void    onGamepadButtonReleaseEvent(int deviceId, QGamepadManager::GamepadButton button);

    void    onGamepadAxisEvent(int deviceId, QGamepadManager::GamepadAxis axis, double value);


private:
    Ui::MapButtonDialog *ui;

    QPushButton*    m_currentButton;
    InputProvider::SNESButton   butVal;
    bool            setMode;

    QMap<InputProvider::SNESButton, QGamepadInputInfos> mapping;
    QString    buttonToText(QGamepadInputInfos);
};

#endif // MAPBUTTONDIALOG_H
