#include "breezeconfig.h"
#include "c5database.h"

BreezeConfig::BreezeConfig(const QStringList &dbParams, int id) :
    Configs(dbParams, 1)
{

}

void BreezeConfig::readValues()
{
    ipAddress = jo["ip"].toString();
    port = jo["port"].toInt();
    apiKey = jo["apikey"].toString();
    username = jo["user"].toString();
    password = jo["password"].toString();

}
