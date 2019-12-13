#include "c5menu.h"

QMap<QString, QMap<QString, QMap<QString, QList<QJsonObject> > > > C5Menu::fMenu;
QMap<QString, int> C5Menu::fPart2Color;
QMap<QString, QString> C5Menu::fMenuNames;
QMap<QString, QStringList> C5Menu::fDishSpecial;
QString C5Menu::fMenuVersion;
QMap<QString, QJsonObject> C5Menu::fPackages;
QMap<int, QList<QJsonObject> > C5Menu::fPackagesList;

C5Menu::C5Menu()
{

}
