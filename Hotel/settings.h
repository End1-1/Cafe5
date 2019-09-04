#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings
{
public:
    Settings();
    inline QVariant v(const QString &key) {return fSettings.value(key); }
    inline QString s(const QString &key) {return fSettings.value(key).toString(); }
    inline int i(const QString &key) {return fSettings.value(key).toInt(); }
    inline void set(const QString &key, const QVariant &value) {fSettings.setValue(key, value);}
    QSettings fSettings;
};

extern Settings __s;

#endif // SETTINGS_H
