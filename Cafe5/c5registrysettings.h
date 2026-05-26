#ifndef C5REGISTRYSETTINGS_H
#define C5REGISTRYSETTINGS_H

#include <QString>

/** QSettings path: Organization + Application\\Module [\\subPath]. No DB / office config. */
class C5RegistrySettings
{
public:
    static QString settingsSubPath;

    static QString registryPath();
};

#endif // C5REGISTRYSETTINGS_H
