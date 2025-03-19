#include "breezeconfig.h"

BreezeConfig::BreezeConfig(const QStringList &dbParams, int id) :
    Configs(dbParams, id)
{
}

void BreezeConfig::readValues()
{
    ipAddress = jo["ip"].toString();
    port = jo["port"].toInt();
    apiKey = jo["apikey"].toString();
    username = jo["user"].toString();
    password = jo["password"].toString();
    dealtype = jo["dealtype"].toString();
}
