#ifndef SKINSELECTOR_H
#define SKINSELECTOR_H

#include "inputdisplay.h"

#include <QListView>
#include <QMainWindow>
#include <QSettings>
#include <QStandardItemModel>

namespace Ui {
class SkinSelector;
}

class SkinSelector : public QMainWindow
{
    Q_OBJECT

public:
    explicit SkinSelector(QWidget *parent = nullptr);
    ~SkinSelector();

private slots:
    void on_startButton_clicked();

    void on_pianoCheckBox_stateChanged(int arg1);

    void on_skinListView_clicked(const QModelIndex &index);

    void    onTelnetConnected();
    void    onTimerTimeout();

    void on_skinPathButton_clicked();

private:
    Ui::SkinSelector *ui;
    QStandardItemModel* listModel;
    QStandardItemModel* pianoModel;
    InputDisplay*       display;
    TelnetConnection*   testCo;
    QTimer              timer;
    QSettings*          m_settings;

    void    setSkinPath(QString path);
    void    addToList(QStandardItemModel* model, QString xmlPath);
    void    setPreviewScene(QString skin);
};

#endif // SKINSELECTOR_H
