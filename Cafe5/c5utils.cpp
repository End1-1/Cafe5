#include "c5utils.h"
#include <QDateTime>

C5Utils __c5utils;

C5Utils::C5Utils()
{
}
QString password(const QString &value)
{
    QByteArray ba(value.toUtf8());
    return QCryptographicHash::hash(ba, QCryptographicHash::Md5).toHex();
}

QString hostusername()
{
    QString name = qEnvironmentVariable("USER");  // Linux/macOS
    if (name.isEmpty()) {
        name = qEnvironmentVariable("USERNAME");  // Windows
    }
    return name;
}

QString columnNumberToLetter(int n)
{
    QString letter = "";
    while (n > 0) {
        n--;  // Уменьшаем на 1, чтобы учитывать нумерацию с A
        letter = char(n % 26 + 'A') + letter;
        n /= 26;
    }
    return letter;
}
