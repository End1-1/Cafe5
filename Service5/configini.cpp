#include "configini.h"

ConfigIni *ConfigIni::fInstance = nullptr;

ConfigIni::ConfigIni()
{

}

void ConfigIni::init()
{
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, "F");
    fInstance = new ConfigIni();
    fInstance->fSettings.setValue("A", "aF");
}

QString ConfigIni::value(const QString &key)
{
    return fInstance->fSettings.value(key, "").toString();
}

bool ConfigIni::isTrue(const QString &key)
{
    return fInstance->value(key).compare("true", Qt::CaseInsensitive) == 0;
}
