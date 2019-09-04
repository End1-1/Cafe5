#include "settings.h"

#include <QHostInfo>

Settings __s;

Settings::Settings() :
    fSettings(_ORGANIZATION_, _APPLICATION_)
{
    QByteArray buf = __s.v("con").toByteArray();
    for (int i = 0; i < buf.length(); i++) {
        buf[i] = buf[i] ^ ((i % 2) + (i % 3) + (i % 4) + (i % 5) + (i % 6) + (i % 7) + (i % 8) + (i % 9));
    }
    QList<QByteArray> s = buf.split(';');
    for (const QString l: s) {
        QStringList kv = l.split('=');
        if (kv.count() != 2) {
            continue;
        }
        set(kv.at(0), kv.at(1));
    }

    QString username = qgetenv("USER");
    if (username.isEmpty())
        username = qgetenv("USERNAME");
    set("username", username.toLower());
    set("hostname", QHostInfo::localHostName().toLower());
}
