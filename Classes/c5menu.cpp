#include "c5menu.h"

C5Menu *C5Menu::fInstance = nullptr;

QMap<QString, QJsonObject> C5Menu::fPackages;
QMap<int, QList<QJsonObject> > C5Menu::fPackagesList;
QMap<int, double> C5Menu::fStopList;
QStringList C5Menu::fDishComments;

C5Menu::C5Menu()
{
}
