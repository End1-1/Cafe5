#include "serverconnection.h"
#include "c5crypt.h"
#include <QSettings>
#include <QByteArray>

ServerConnection *ServerConnection::fInstance = nullptr;

ServerConnection::ServerConnection()
{
    QSettings s(_ORGANIZATION_, _APPLICATION_);
    QByteArray in = s.value("data").toByteArray();
    QByteArray out;
    C5Crypt c;
    c.ede3_cbc(in, out, _CRYPT_KEY_, false);
    QStringList outList = QString(out).split(";", QString::SkipEmptyParts);
    if (outList.count() == 4) {
        fIP = outList.at(0);
        fPort = outList.at(1);
        fUsername = outList.at(2);
        fPassword = outList.at(3);
    }
}

void ServerConnection::getParams(QString &ip, int &port, QString &username, QString &password)
{
    if (!fInstance) {
        fInstance = new ServerConnection();
    }
    ip = fInstance->fIP;
    port = fInstance->fPort.toInt();
    username = fInstance->fUsername;
    password = fInstance->fPassword;
}

void ServerConnection::setParams(const QString &ip, const QString &port, const QString &username, const QString &password)
{
    if (!fInstance) {
        fInstance = new ServerConnection();
    }
    QByteArray in = QString("%1;%2;%3;%4").arg(ip, port, username, password).toLatin1();
    QByteArray out;
    C5Crypt c;
    c.ede3_cbc(in, out, _CRYPT_KEY_, true);
    QSettings s(_ORGANIZATION_, _APPLICATION_);
    s.setValue("data", out);
    fInstance->fIP = ip;
    fInstance->fPort = port;
    fInstance->fUsername = username;
    fInstance->fPassword = password;
}
