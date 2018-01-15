#ifndef SKINSELECTOR_H
#define SKINSELECTOR_H

#include <QMainWindow>

namespace Ui {
class SkinSelector;
}

class SkinSelector : public QMainWindow
{
    Q_OBJECT

public:
    explicit SkinSelector(QWidget *parent = 0);
    ~SkinSelector();

private:
    Ui::SkinSelector *ui;
};

#endif // SKINSELECTOR_H
