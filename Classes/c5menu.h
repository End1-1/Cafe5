#ifndef C5MENU_H
#define C5MENU_H

#include <QString>
#include <QMap>
#include <QJsonObject>
#include <QColor>

class C5Menu
{
public:
    C5Menu();
    /*          Menu          Part1         Part2       Dishes */
    static QMap<QString, QMap<QString, QMap<QString, QList<QJsonObject> > > >  fMenu;

    static QMap<QString, int> fPart2Color;

    static QMap<QString, QString> fMenuNames;

    static QMap<QString, QStringList> fDishSpecial;

    static QString fMenuVersion;

    static QMap<QString, QJsonObject> fPackages;

    static QMap<int, QList<QJsonObject> > fPackagesList;

    static QMap<int, double> fStopList;
};

#endif // C5MENU_H
