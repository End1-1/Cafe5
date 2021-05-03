#include "configini.h"
#include <QDir>

ConfigIni *ConfigIni::fInstance = nullptr;

ConfigIni::ConfigIni()
{

}

ConfigIni::~ConfigIni()
{
    delete fSettings;
}

void ConfigIni::init()
{
    fInstance = new ConfigIni();
    fInstance->fSettings = new QSettings(QDir::currentPath() + "/config.ini", QSettings::IniFormat);
}

QString ConfigIni::value(const QString &key)
{
    return fInstance->fSettings->value(key, "").toString();
}

bool ConfigIni::isTrue(const QString &key)
{
    return fInstance->value(key).compare("true", Qt::CaseInsensitive) == 0;
}
