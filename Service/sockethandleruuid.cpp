#include "sockethandleruuid.h"
#include "c5database.h"
#include "config.h"
#include <QJsonObject>
#include <QJsonDocument>

SocketHandlerUUID::SocketHandlerUUID(SocketData *sd, QByteArray &data) :
    SocketHandler(sd, data)
{

}

void SocketHandlerUUID::processData()
{
    QJsonObject jo = QJsonDocument::fromJson(fData).object();
    C5Database db(DBHOST, DBFILE, DBUSER, DBPASSWORD);
    QString id = db.uuid();
    QJsonObject j;
    j["id"] = jo["id"].toInt();
    j["uuid"] = id;
    fData = QJsonDocument(j).toJson(QJsonDocument::Compact);
}
