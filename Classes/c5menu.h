#ifndef C5MENU_H
#define C5MENU_H

#include "datadriver.h"
#include <QString>
#include <QMap>
#include <QJsonObject>
#include <QColor>

#define menu5 C5Menu::fInstance

class DDish {
public:
    QList<int> data;
};

class DPart2 {
public:
    DPart2(int id):fId(id){}
    QList<DPart2> data1;
    DDish data2;
    int fId;
};

class DPart1 {
public:
    QList<DPart2> data;

    DPart2 &part2(int id, int dishid) {
        for (int i = 0; i < data.count(); i++) {
            if (data.at(i).fId == id) {
                if (dishid > 0) {
                    data[i].data2.data.append(dishid);
                }
                return data[i];
            }
        }
        if (dbdishpart2->parentid(id) == 0) {
            data.append(DPart2(id));
            return part2(id, dishid);
        }
        DPart2 p2(id);
        if (dishid > 0) {
            p2.data2.data.append(dishid);
        }
        DPart2 root = findParent(dbdishpart2->parentid(id), p2);
        bool newtree = true;
        for (int i = 0; i < data.count(); i++) {
            if (data.at(i).fId == root.fId) {
                addToTree(data[i], root.data1[0]);
                newtree = false;
            }
        }
        if (newtree) {
            data.append(root);
        }
        return part2(root.fId, 0);
    }

    bool addToTree(DPart2 &l1, DPart2 &l2) {
        for (int i = 0; i < l1.data1.count(); i++) {
            if (l1.data1.at(i).fId == l2.fId) {
                if (l1.data1.count() > 0 && l2.data1.count() > 0) {
                    if (addToTree(l1.data1[i], l2.data1[0])) {
                        return true;
                    }
                }
                if (l1.data1.count() == 0) {
                    l1.data1.append(l2);
                    return true;
                }
                if (l2.data1.count() == 0) {
                    l1.data1[i].data2.data.append(l2.data2.data);
                    return true;
                }

            }
        }
        l1.data1.append(l2);
    }

    DPart2 findParent(int id, DPart2 child) {
        if (id == 0) {
            return child;
        } else {
            DPart2 parent(id);
            parent.data1.append(child);
            DPart2 root = findParent(dbdishpart2->parentid(id), parent);
            return root;
        }
    }
};

class DMenu {
public:
    QMap<int, QMap<int, DPart1> > data;

    void check(int id) {
        if (!data.contains(id)) {
            data.insert(id, QMap<int, DPart1>());
        }
    }

    DPart1 &part1(int menuid, int part1id) {
        if (!data[menuid].contains(part1id)) {
            data[menuid].insert(part1id, DPart1());
        }
        return data[menuid][part1id];
    }
};

class C5Menu
{
public:
    C5Menu();
    /*          Menu          Part1         Part2       Dishes */
    //static QMap<QString, QMap<QString, QMap<QString, QList<QJsonObject> > > >  fMenu;

    DMenu fMenuList;

    QMap<int, int> fPart2Color;

    static QStringList fDishComments;

    static QMap<QString, QString> fMenuNames;

    static QString fMenuVersion;

    static QMap<QString, QJsonObject> fPackages;

    static QMap<int, QList<QJsonObject> > fPackagesList;

    static QMap<int, double> fStopList;

    void refresh();

    static C5Menu *fInstance;
};

#endif // C5MENU_H
