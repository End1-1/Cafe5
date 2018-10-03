#ifndef C5MENU_H
#define C5MENU_H

#include <QString>
#include <QMap>
#include <QJsonObject>

typedef struct  {
    int fId;
    QString fName;
} DPART1;

class C5Menu
{
public:
    C5Menu();
    /*          Menu          Part1         Part2       Dishews */
    static QMap<QString, QMap<QString, QMap<QString, QList<QJsonObject> > > >  fMenu;
};

#endif // C5MENU_H
