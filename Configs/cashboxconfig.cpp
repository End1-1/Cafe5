#include "cashboxconfig.h"

CashboxConfig::CashboxConfig(const QStringList &dbParams, int id) :
    Configs(dbParams, id)
{

}

void CashboxConfig::readValues()
{
    cash1 = jo["cash1"].toInt();
    cash2 = jo["cash2"].toInt();
    cash3 = jo["cash3"].toInt();
    cash4 = jo["cash4"].toInt();
    cash5 = jo["cash5"].toInt();
    cash6 = jo["cash6"].toInt();
    cash7 = jo["cash7"].toInt();

    cash1Name = jo["cash1Name"].toString();
    cash2Name = jo["cash2Name"].toString();
    cash3Name = jo["cash3Name"].toString();
    cash4Name = jo["cash4Name"].toString();
    cash5Name = jo["cash5Name"].toString();
    cash6Name = jo["cash6Name"].toString();
    cash7Name = jo["cash7Name"].toString();

    cash1Enabled = jo["cash1Enabled"].toBool();
    cash2Enabled = jo["cash2Enabled"].toBool();
    cash3Enabled = jo["cash3Enabled"].toBool();
    cash4Enabled = jo["cash4Enabled"].toBool();
    cash5Enabled = jo["cash5Enabled"].toBool();
    cash6Enabled = jo["cash6Enabled"].toBool();
    cash7Enabled = jo["cash7Enabled"].toBool();

    cash6Debt = jo["cash6Debt"].toBool();
    cash6Present = jo["cash6Present"].toBool();
    cash6Hotel = jo["cash6Hotel"].toBool();
    cash6Selfcost = jo["cash6Selfcost"].toBool();
}
