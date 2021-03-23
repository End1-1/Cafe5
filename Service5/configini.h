#ifndef CONFIGINI_H
#define CONFIGINI_H

#include <QSettings>

class ConfigIni
{
public:
    ConfigIni();
    static void init();
    static QString value(const QString &key);
    static bool isTrue(const QString &key);

private:
    static ConfigIni *fInstance;
    QSettings fSettings;
};

#endif // CONFIGINI_H
