#include "sockethandlerserviceconfig.h"
#include "servicecommands.h"
#include "config.h"
#include <QSettings>
#include <QJsonDocument>
#include <QCryptographicHash>
#include <QJsonObject>

SocketHandlerServiceConfig::SocketHandlerServiceConfig(SocketData *sd, QByteArray &data) :
    SocketHandler(sd, data)
{

}

void SocketHandlerServiceConfig::processData()
{
    QJsonDocument jdoc = QJsonDocument::fromJson(fData);
    QJsonObject jo = jdoc.object();
    if (ADMINPASS.toLower() == QString(QCryptographicHash::hash(jo["adminpass"].toString().toLower().toUtf8(), QCryptographicHash::Md5).toHex())) {
        ADMINPASS = QString(QCryptographicHash::hash(jo["dbpass"].toString().toLower().toUtf8(), QCryptographicHash::Md5).toHex());
        QSettings s(_ORGANIZATION_, _APPLICATION_+ QString("\\") + _MODULE_);
        s.setValue("dbhost", jo["dbhost"].toString());
        s.setValue("dbfile", jo["dbfile"].toString());
        s.setValue("dbuser", jo["dbuser"].toString());
        s.setValue("dbpass", jo["dbpass"].toString());
        INITCONFIG();
        fData.clear();
    } else {
        fResponseCode = dr_service_login_failed;
    }
}
