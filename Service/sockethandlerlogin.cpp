#include "sockethandlerlogin.h"
#include "sockethandlergoods.h"
#include "c5database.h"
#include "config.h"
#include "servicecommands.h"
#include <QJsonDocument>
#include <QJsonObject>

SocketHandlerLogin::SocketHandlerLogin(SocketData *sd, QByteArray &data) :
    SocketHandler(sd, data)
{

}

bool SocketHandlerLogin::login()
{
    QJsonDocument jdoc = QJsonDocument::fromJson(fData);
    QJsonObject jo = jdoc.object();
    C5Database db(DBHOST, DBFILE, DBUSER, DBPASSWORD);
    if (!jo["session"].toString().isEmpty()) {
        QByteArray session =  QByteArray::fromBase64(jo["session"].toString().toUtf8());
        db[":f_iplogout"] = fPeerAddress;
        db[":f_dateend"] = QDate::currentDate();
        db[":f_timeend"] = QTime::currentTime();
        db[":f_session"] = session;
        db.exec("update s_login_session set f_iplogout=:f_iplogout, f_dateend=:f_dateend, f_timeend=:f_timeend where f_session=:f_session");
        if (jo.contains("logout")) {
            fResponseCode = dr_logout;
            return true;
        }
    }
    bool listOfGoods = jo["listofgoods"].toInt() == 1;
    db[":f_login"] = jo["username"].toString();
    db[":f_password"] = jo["password"].toString();
    db.exec("select f_id, f_group, f_first, f_last, f_config from s_user where f_login=:f_login and f_password=md5(:f_password)");
    if (db.nextRow()) {
        fData.clear();
        jo = QJsonObject();
        fSocketData->fUserId = db.getInt("f_id");
        jo["id"] = fSocketData->fUserId;
        jo["group"] = db.getInt("f_group");
        jo["firstname"] = db.getString("f_first");
        jo["lastname"] = db.getString("f_last");
        int configId = db.getInt("f_config");
        fSocketData->fSessionId = db.uuid_bin();
        jo["session"] = QString(fSocketData->fSessionId.toBase64());
        db[":f_session"] = fSocketData->fSessionId;
        db[":f_user"] = jo["id"].toInt();
        db[":f_iplogin"] = fPeerAddress;
        db[":f_datestart"] = QDate::currentDate();
        db[":f_timestart"] = QTime::currentTime();
        db.insert("s_login_session", false);
        if (listOfGoods) {
            QByteArray goodsData;
            SocketHandlerGoods shg(fSocketData, goodsData);
            shg.processData();
            jo["listofgoodsgroups"] = QJsonDocument::fromJson(goodsData).object();
        }
        jdoc = QJsonDocument(jo);
        fData = jdoc.toJson(QJsonDocument::Compact);
        db[":f_settings"] = configId;
        db.exec("select f_key, f_value from s_settings_values where f_settings=:f_settings");
        while (db.nextRow()) {
            fSocketData->fSettings[db.getInt(0)] = db.getString("f_value");
        }
    } else {
        fResponseCode = dr_login_failed;
    }
    return fResponseCode == dr_ok;
}

void SocketHandlerLogin::processData()
{

}
