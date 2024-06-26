#include "configini.h"
#include <QDir>
#include <QDebug>

ConfigIni *ConfigIni::fInstance = nullptr;
QString ConfigIni::fAppPath;

ConfigIni::ConfigIni()
{

}

ConfigIni::~ConfigIni()
{
    delete fSettings;
}

void ConfigIni::init(const QString &path)
{
    fInstance = new ConfigIni();
    fInstance->fSettings = new QSettings(path, QSettings::IniFormat);
    fAppPath = path;
}

QString ConfigIni::value(const QString &key)
{
    return fInstance->fSettings->value(key, "").toString();
}

bool ConfigIni::isTrue(const QString &key)
{
    return fInstance->value(key).compare("true", Qt::CaseInsensitive) == 0;
}

void ConfigIni::setValue(const QString &key, const QString &value)
{
    fInstance->fSettings->setValue(key, value);
}
