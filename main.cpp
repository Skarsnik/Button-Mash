#include "skinselector.h"
#include "../telnetconnection.h"
#include <QApplication>
#include "inputdisplay.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    InputDisplay d("D:/Project/snesclassicstuff/desktopclient/InputDisplay/Skins/snes-Derpy/skin.xml");
    //SkinSelector w;
    //w.show();
    d.show();
    return a.exec();
}
