#include "skinselector.h"
#include <QApplication>
#include "mapbuttondialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SkinSelector w;
    w.show();
    /*MapButtonDialog diag;
    diag.show();*/
    return a.exec();
}
