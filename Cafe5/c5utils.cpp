#include "c5utils.h"
#include "c5config.h"
#include <QFile>
#include <QDateTime>

#ifdef WIN32
#include <windows.h>
#endif

C5Utils __c5utils;

C5Utils::C5Utils()
{

}

void C5Utils::writeErrorLog(const QString &text)
{
    QFile f(__c5config.fAppLogFile);
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
#ifdef WIN32
    wchar_t userName[100];
    DWORD userNameSize = sizeof(userName);
    memset(&userName[0], 0, userNameSize);
    GetUserName(userName, &userNameSize);
    return QString::fromWCharArray(userName);
#endif
    return "Unknown";
}
