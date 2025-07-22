#include "breezeconfig.h"

BreezeConfig::BreezeConfig(int id) :
    Configs(id)
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
