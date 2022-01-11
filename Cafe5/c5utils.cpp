#include "c5utils.h"
#include "c5config.h"
#include <QFile>
#include <QDateTime>

C5Utils __c5utils;

C5Utils::C5Utils()
{

}

void C5Utils::writeErrorLog(const QString &text)
{
    QFile f("c:\\Intel\\log.txt");
    if (f.open(QIODevice::Append)) {
        f.write(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR).toUtf8());
        f.write(" ");
        f.write(text.toUtf8());
        f.write("\r\n");
        f.close();
    }
}

QString password(const QString &value)
{
    QByteArray ba(value.toUtf8());
    return QCryptographicHash::hash(ba, QCryptographicHash::Md5).toHex();
}

QString hostusername()
{
    QString name = getenv("USER");
    if (name.isEmpty()) {
        name = getenv("USERNAME");
    }
    return name;
}
