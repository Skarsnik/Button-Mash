#ifndef SKINEDITOR_H
#define SKINEDITOR_H

#include <QDir>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPushButton>
#include <QButtonGroup>
#include "inputprovider.h"
#include "skinparser.h"

namespace Ui {
class SkinEditor;
}

class SkinEditor : public QWidget
{
    Q_OBJECT

public:
    explicit SkinEditor(QWidget *parent = nullptr);
    ~SkinEditor();
    void    openSkin(const QDir& dir);
    void    openSkin(const QString xmlPath);

private slots:
    void on_openDir_clicked();

    void on_addButtonButton_clicked();

private:
    Ui::SkinEditor *ui;

    QMap<QString, QGraphicsPixmapItem*> mapItems;
    QMap<InputProvider::SNESButton, QString> mapButtonToText;
    QMap<QPushButton*, InputProvider::SNESButton> mapButtonToId;
    QButtonGroup*   buttonGroup;
    QDir            skinDir;
    RegularSkin     skin;
    QGraphicsScene* scene;
    QString         defaultSkinPath;

    void     onSnesButtonClicked(QAbstractButton* but);
    void setCurrentButton(int id);
};

#endif // SKINEDITOR_H
