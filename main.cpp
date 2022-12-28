#include "skineditor.h"
#include "skinselector.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SkinSelector w;
    /*SkinEditor e;
    e.show();*/

    w.show();
    /*MapButtonDialog diag;
    diag.show();*/
    return a.exec();
}
