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

private:
    Ui::SkinEditor *ui;
};

#endif // SKINEDITOR_H
