#include "skinselector.h"
#include "../telnetconnection.h"
#include <QApplication>
#include "inputdisplay.h"
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
