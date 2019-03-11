#ifndef SKINSELECTOR_H
#define SKINSELECTOR_H

#include "inputdisplay.h"
#include "skinparser.h"
#include "telnetconnection.h"
#include "inputdecoder.h"
#include "inputsourceselector.h"

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

    void    onTimerTimeout();

    void on_skinPathButton_clicked();

    void on_subSkinListView_clicked(const QModelIndex &index);

    void    onDisplayClosed();

    void on_changeSourceButton_clicked();


    void on_configHSButton_clicked();

private:
    Ui::SkinSelector *ui;
    QStandardItemModel* listModel;
    QStandardItemModel* pianoModel;
    QStandardItemModel* subSkinModel;
    InputDisplay*       display;
    InputProvider*      inputProvider;
    QTimer              timer;
    QSettings*          m_settings;
    RegularSkin         currentSkin;
    InputSourceSelector* inputSelector;

    void    setSkinPath(QString path);
    void    setPreviewScene(const RegularSkin &skin);
    void    saveSkinStarted();
    void    restoreLastSkin();
};

#endif // SKINSELECTOR_H
