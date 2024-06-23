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

    QMap<int, int> fPart2Color;

    static QStringList fDishComments;

    static QMap<QString, QString> fMenuNames;

    static QMap<QString, QJsonObject> fPackages;

    static QMap<int, QList<QJsonObject> > fPackagesList;

    static QMap<int, double> fStopList;

    static C5Menu *fInstance;
};

#endif // C5MENU_H
