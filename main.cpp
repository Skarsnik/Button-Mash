#include "skinselector.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SkinSelector w;
    w.show();
    /*MapButtonDialog diag;
    diag.show();*/
    return a.exec();
}
