#include "c5menu.h"

C5Menu *C5Menu::fInstance = nullptr;

QMap<QString, QString> C5Menu::fMenuNames;
QString C5Menu::fMenuVersion;
QMap<QString, QJsonObject> C5Menu::fPackages;
QMap<int, QList<QJsonObject> > C5Menu::fPackagesList;
QMap<int, double> C5Menu::fStopList;

C5Menu::C5Menu()
{

}

void C5Menu::refresh()
{
    dbmenu->refresh();
    dbmenuname->refresh();
    dbdish->refresh();
    dbdishpart1->refresh();
    dbdishpart2->refresh();
    dbdishspecial->refresh();
    for (int id: dbmenu->list()) {
        int dishid = dbmenu->dishid(id);
        int part2id = dbdish->part2(dishid);
        int part1id = dbdishpart2->part1(part2id);
        int menuid = dbmenu->menuid(id);

        menu5->fMenuList.check(menuid);
        DPart1 &part1 = menu5->fMenuList.part1(menuid, part1id);
        DPart2 &part2 = part1.part2(part2id, id);
        Q_ASSERT(part2.fId > 0);
    }
}
