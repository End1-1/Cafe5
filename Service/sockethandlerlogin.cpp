#include "sockethandlerlogin.h"
#include "c5database.h"
#include "config.h"
#include "servicecommands.h"
#include <QJsonDocument>
#include <QJsonObject>

SocketHandlerLogin::SocketHandlerLogin(QByteArray &data) :
    SocketHandler(data)
{

}

void SocketHandlerLogin::processData()
{
    QJsonDocument jdoc = QJsonDocument::fromJson(fData);
    QJsonObject jo = jdoc.object();
    C5Database db(DBHOST, DBFILE, DBUSER, DBPASSWORD);
    db[":f_login"] = jo["username"].toString();
    db[":f_password"] = jo["password"].toString();
    db.exec("select f_id, f_group, f_first, f_last from s_user where f_login=:f_login and f_password=md5(:f_password)");
    if (db.nextRow()) {
        fData.clear();
        jo = QJsonObject();
        jo["id"] = db.getInt("f_id");
        jo["group"] = db.getInt("f_group");
        jo["firstname"] = db.getString("f_first");
        jo["lastname"] = db.getString("f_last");
        jdoc = QJsonDocument(jo);
        fData = jdoc.toJson(QJsonDocument::Compact);
    } else {
        fResponseCode = dr_login_failed;
    }
}
