#ifndef SKINPARSER_H
#define SKINPARSER_H

#include <QColor>
#include <QMap>
#include <QString>
#include <QDebug>

struct   RegularButtonSkin
{
    QString name;
    QString image;
    int x;
    int y;
    int width;
    int height;
};

QDebug              operator<<(QDebug debug, const RegularButtonSkin& but);

struct   RegularSkin
{
    QString file;
    QString name;
    QString author;
    QString background;
    QList<RegularSkin> subSkins;
    QMap<QString, RegularButtonSkin>    buttons;
};

Q_DECLARE_METATYPE(RegularSkin)

QDebug              operator<<(QDebug debug, const RegularSkin& req);

struct PianoButton
{
    QString name;
    int x;
    int width;
    QColor  color;
};

QDebug              operator<<(QDebug debug, const PianoButton& pb);

struct  PianoSkin
{
    QString file;
    QString name;
    QString author;
    QColor  bgcolor;
    QString background;
    int     width;
    int     height;
    QMap<QString, PianoButton>  buttons;
};

QDebug              operator<<(QDebug debug, const PianoSkin& pk);

Q_DECLARE_METATYPE(PianoSkin)

class SkinParser
{
public:
    SkinParser();
    static RegularSkin          parseRegularSkin(QString filePath);
    static PianoSkin            parsePianoSkin(QString filePath);
    static  QString             xmlError;
    static  int                 lineNumber;
    static  int                 columNumber;
};


#endif // SKINPARSER_H
