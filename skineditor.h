#ifndef SKINEDITOR_H
#define SKINEDITOR_H

#include <QDir>
#include <QWidget>

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

private:
    Ui::SkinEditor *ui;

    QDir   skinDir;
};

#endif // SKINEDITOR_H
