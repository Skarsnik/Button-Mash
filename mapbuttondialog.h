#ifndef MAPBUTTONDIALOG_H
#define MAPBUTTONDIALOG_H


#include <QAbstractButton>
#include <QDialog>
#include <QGamepadManager>
#include <QTimer>

#include "inputprovider.h"
#include "localcontrollermanager.h"


namespace Ui {
class MapButtonDialog;
}

class MapButtonDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MapButtonDialog(QWidget *parent = nullptr);
    void     setDevice(LocalControllerInfos device);
    void     setMapping(QMap<InputProvider::SNESButton, LocalControllerButtonAxisInfos> mapping);
    QMap<InputProvider::SNESButton, LocalControllerButtonAxisInfos> mapping() const;
    ~MapButtonDialog();


private slots:
    void    onButtonGroupClicked(QAbstractButton* button);
    void    onGamepadButtonPressEvent(int deviceId, QGamepadManager::GamepadButton button, double value);
    void    onGamepadButtonReleaseEvent(int deviceId, QGamepadManager::GamepadButton button);

    void    onGamepadAxisEvent(int deviceId, QGamepadManager::GamepadAxis axis, double value);

    void    onDirectInputButtonEvent(QGameControllerButtonEvent* event);
    void    onDirectInputAxisEvent(QGameControllerAxisEvent* event);

private:
    Ui::MapButtonDialog *ui;

    QPushButton*    m_currentButton;
    InputProvider::SNESButton   butVal;
    bool            setMode;
    int             qGamepadDevice_id;
    int             directInputDevice_id;
    QGameController* directInputDevice;
    QTimer          directInputTimer;

    QMap<InputProvider::SNESButton, LocalControllerButtonAxisInfos> m_mapping;
    QString    buttonToText(LocalControllerButtonAxisInfos);
};

#endif // MAPBUTTONDIALOG_H
