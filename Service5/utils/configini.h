#ifndef CONFIGINI_H
#define CONFIGINI_H

#include <QSettings>
#include <QString>

class ConfigIni
{
public:
    ConfigIni();
    ~ConfigIni();
    static QString fAppPath;
    static void init(const QString &path);
    static QString value(const QString &key);
    static bool isTrue(const QString &key);
    static void setValue(const QString &key, const QString &value);

private:
    static ConfigIni *fInstance;
    QSettings *fSettings;
};

#endif // CONFIGINI_H
