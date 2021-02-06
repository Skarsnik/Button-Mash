#ifndef MAPBUTTONDIALOG_H
#define MAPBUTTONDIALOG_H


#include <QAbstractButton>
#include <QDialog>
#include <QGamepadManager>

#include "inputprovider.h"
#include "qgamepadinputinfos.h"


namespace Ui {
class MapButtonDialog;
}

class MapButtonDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MapButtonDialog(QWidget *parent = nullptr);
    void                                                setMapping(QMap<InputProvider::SNESButton, QGamepadInputInfos> mapping);
    QMap<InputProvider::SNESButton, QGamepadInputInfos> mapping() const;
    ~MapButtonDialog();


private slots:
    void    onButtonGroupClicked(QAbstractButton* button);
    void    onGamepadButtonPressEvent(int deviceId, QGamepadManager::GamepadButton button, double value);
    void    onGamepadButtonReleaseEvent(int deviceId, QGamepadManager::GamepadButton button);

    void    onGamepadAxisEvent(int deviceId, QGamepadManager::GamepadAxis axis, double value);


private:
    Ui::MapButtonDialog *ui;

    QPushButton*    m_currentButton;
    InputProvider::SNESButton   butVal;
    bool            setMode;

    QMap<InputProvider::SNESButton, QGamepadInputInfos> m_mapping;
    QString    buttonToText(QGamepadInputInfos);
};

#endif // MAPBUTTONDIALOG_H
