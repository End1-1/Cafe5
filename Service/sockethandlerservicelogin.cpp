#include "sockethandlerservicelogin.h"
#include "config.h"
#include "servicecommands.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>

SocketHandlerServiceLogin::SocketHandlerServiceLogin(SocketData *sd, QByteArray &data) :
    SocketHandler(sd, data)
{

}

void SocketHandlerServiceLogin::processData()
{
    QJsonDocument jdoc = QJsonDocument::fromJson(fData);
    QJsonObject jo = jdoc.object();
    fData.clear();
    QString pass = QString(QCryptographicHash::hash(jo["adminpass"].toString().toLower().toUtf8(), QCryptographicHash::Md5).toHex());
    if (ADMINPASS.toLower() == pass) {
        jo = QJsonObject();
        jo["dbhost"] = DBHOST;
        jo["dbfile"] = DBFILE;
        jo["dbuser"] = DBUSER;
        jdoc = QJsonDocument(jo);
        fData.append(jdoc.toJson(QJsonDocument::Compact));
    } else {
        fResponseCode = dr_service_login_failed;
        fData = "Login failed";
    }
}
