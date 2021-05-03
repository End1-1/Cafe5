#include "c5license.h"
#include "c5crypt.h"
#include <QDate>
#include <QDebug>
#include <QSysInfo>

//static const QString settingsName(QString(_ORGANIZATION_), QString("%1\\%2").arg(_APPLICATION_).arg(_MODULE_));

C5License::C5License()
{

}

bool C5License::isOK()
{
    qDebug() << QSysInfo::machineUniqueId();
    C5Crypt c;
    QString key = "theKey";
    QByteArray in = "sceleton";
    QByteArray out;
    c.cryptData(key.toUtf8(), in, out);
    qDebug() << key << in << out;
    c.decryptData(key.toUtf8(), out, in);
    qDebug() << key << in << out;
    if (QDate::currentDate() > QDate::fromString("01.06.2021", "dd.MM.yyyy")) {
        return false;
    }
    return true;    
}
