#include "skinparser.h"

#include <QDomDocument>
#include <QFile>
#include <QDebug>
#include <QPixmap>
#include <QFileInfo>

QString SkinParser::xmlError = QString();
int     SkinParser::lineNumber = 0;
int     SkinParser::columNumber = 0;

SkinParser::SkinParser()
{

}

RegularSkin SkinParser::parseRegularSkin(QString filePath)
{
    xmlError.clear();
    QDomDocument doc;
    RegularSkin toret;
    QFileInfo fi(filePath);
    QFile skinFile(filePath);
    skinFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString xmlSkin = skinFile.readAll();
    if (!doc.setContent(xmlSkin, true, &xmlError, &lineNumber,
                                   &columNumber))
        return toret;
    QDomElement root = doc.documentElement();
    QDomElement child = root.firstChildElement();
    toret.author = root.attribute("author");
    toret.name = root.attribute("name");

    QMap<QString, RegularButtonSkin> defaultBut;
    RegularSkin* currentSkin = &toret;
    QString firstBgName;
    bool    notFirstBg = false;
    while (!child.isNull())
    {
        if (child.tagName() == "background")
        {
            if (notFirstBg)
            {
                qDebug() << "Sub Skin";
                if (toret.subSkins.isEmpty())
                {
                    toret.subSkins.append(toret);
                    toret.subSkins.first().name = firstBgName;
                }
                toret.subSkins.append(RegularSkin());
                currentSkin = &toret.subSkins.last();
                currentSkin->name = child.attribute("name");
            }
            if (firstBgName.isEmpty())
                firstBgName = child.attribute("name");
            currentSkin->file = filePath;
            currentSkin->author = toret.author;
            currentSkin->background = child.attribute("image");
            notFirstBg = true;
            qDebug() << currentSkin->name;
        }
        if (child.tagName() == "button")
        {
            RegularButtonSkin but;

            but.image = child.attribute("image");
            QPixmap pix(fi.absolutePath() + "/" + but.image);
            if (pix.isNull())
            {
                xmlError = "Can't open " + fi.absolutePath() + "/" + but.image;
                return toret;
            }
            but.width = pix.width();
            but.height = pix.height();
            if (!child.attribute("width").isEmpty())
                 but.width = child.attribute("width").toUInt();
            if (!child.attribute("height").isEmpty())
                 but.height = child.attribute("height").toUInt();
            but.x = child.attribute("x").toInt();
            but.y = child.attribute("y").toInt();
            but.name = child.attribute("name");
            defaultBut[child.attribute("name")] = but;
            currentSkin->buttons[child.attribute("name")] = but;
        }
        child = child.nextSiblingElement();
    }
    qDebug() << toret.name << toret.buttons.isEmpty();
    QMutableListIterator<RegularSkin> it(toret.subSkins);
    while (it.hasNext())
    {
        RegularSkin& sk = it.next();
        if (sk.buttons.isEmpty())
            sk.buttons = defaultBut;
    }
    if (toret.buttons.isEmpty() && !toret.subSkins.isEmpty())
    {
        qDebug() << "Setting default button" << toret.subSkins.first().buttons.size();
        toret.buttons = toret.subSkins.first().buttons;
    }
    return toret;
}

PianoSkin SkinParser::parsePianoSkin(QString filePath)
{
    xmlError.clear();
    QDomDocument doc;
    PianoSkin toret;
    QFileInfo fi(filePath);
    QFile skinFile(filePath);
    skinFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString xmlSkin = skinFile.readAll();
    if (!doc.setContent(xmlSkin, true, &xmlError, &lineNumber,
                                   &columNumber))
        return toret;
    QDomElement root = doc.documentElement();
    QDomElement child = root.firstChildElement();
    toret.file = fi.absoluteFilePath();
    toret.author = root.attribute("author");
    toret.name = root.attribute("name");
    QDomElement mainarea = root.firstChildElement();
    toret.width = mainarea.attribute("width").toInt();
    toret.height = mainarea.attribute("height").toInt();
    QColor col(mainarea.attribute("backgroundcolor"));
    if (!col.isValid())
    {
        xmlError = "Invalid background color";
        return toret;
    }
    toret.bgcolor = col;
    QDomElement button = mainarea.firstChildElement();
    while (!button.isNull())
    {
        PianoButton but;
        but.name = button.attribute("name");
        but.x = button.attribute("x").toInt();
        but.width = button.attribute("width").toInt();
        QColor c2(button.attribute("color"));
        if (!c2.isValid())
        {
            xmlError = "Invalid button color";
            lineNumber = button.lineNumber();
            return toret;
        }
        but.color = c2;
        toret.buttons[but.name] = but;
        button = button.nextSiblingElement();
    }
    return toret;
}

QDebug operator<<(QDebug debug, const RegularSkin &req)
{
    debug << "Author : " << req.author;
    debug << "Name : " << req.name;
    debug << "Buttons : \n";
    foreach(RegularButtonSkin but, req.buttons)
    {
        debug << but;
    }
    debug << "\nNumber of subskin" << req.subSkins.size();
    foreach(RegularSkin sk, req.subSkins)
    {
        debug << "\n====\n";
        debug << "    " << "Name" << sk.name;
        debug << "    " << "Buttons : " << sk.buttons.size() << "\n";
        foreach(RegularButtonSkin but, sk.buttons)
        {
            debug << but;
        }
    }
    return debug;
}

QDebug operator<<(QDebug debug, const RegularButtonSkin &but)
{
    debug << "Name" << but.name << but.image << but.x << but.y << but.width << but.height;
    debug << "\n";
    return debug;
}

QDebug operator<<(QDebug debug, const PianoButton &pb)
{
    debug << "Name" << pb.name << "x :" << pb.x << "width : " << pb.width << pb.color << "\n";
    return debug;
}

QDebug operator<<(QDebug debug, const PianoSkin &pk)
{
    debug << "Name " << pk.name << " by" << pk.author << "\n";
    foreach(PianoButton but, pk.buttons)
    {
        debug << but;
    }
    return debug;
}
