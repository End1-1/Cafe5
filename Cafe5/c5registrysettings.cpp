#include "c5registrysettings.h"

QString C5RegistrySettings::settingsSubPath;

QString C5RegistrySettings::registryPath()
{
    QString path = QString::fromUtf8(_APPLICATION_) + QLatin1Char('\\') + QString::fromUtf8(_MODULE_);
    if (!settingsSubPath.isEmpty()) {
        path += QLatin1Char('\\') + settingsSubPath;
    }
    return path;
}
