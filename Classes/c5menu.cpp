#include "c5menu.h"

QMap<QString, QMap<QString, QMap<QString, QList<QJsonObject> > > > C5Menu::fMenu;
QMap<QString, int> C5Menu::fPart2Color;
QMap<QString, QString> C5Menu::fMenuNames;

C5Menu::C5Menu()
{

}
