#ifndef INPUTSOURCESELECTOR_H
#define INPUTSOURCESELECTOR_H

#include <QWidget>

namespace Ui {
class InputSourceSelector;
}

class InputSourceSelector : public QWidget
{
    Q_OBJECT

public:
    explicit InputSourceSelector(QWidget *parent = nullptr);
    ~InputSourceSelector();

private:
    Ui::InputSourceSelector *ui;
};

#endif // INPUTSOURCESELECTOR_H
