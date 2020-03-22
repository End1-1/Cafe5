#include "sockethandlerloginwithsession.h"
#include "c5database.h"
#include "config.h"
#include "sockethandlergoods.h"
#include "servicecommands.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

SocketHandlerLoginWithSession::SocketHandlerLoginWithSession(SocketData *sd, QByteArray &data) :
    SocketHandler(sd, data)
{

}

bool SocketHandlerLoginWithSession::login()
{
    QJsonDocument jdoc = QJsonDocument::fromJson(fData);
    QJsonObject jo = jdoc.object();
    QByteArray session =  QByteArray::fromBase64(jo["session"].toString().toUtf8());
    C5Database db(DBHOST, DBFILE, DBUSER, DBPASSWORD);
    bool listOfGoods = jo["listofgoods"].toInt() == 1;
    db[":f_session"] = session;
    db.exec("select f_id, f_user from s_login_session where f_session=:f_session");
    fSocketData->fUserId = 0;
    if (db.nextRow()) {
        int recid = db.getInt("f_id");
        fSocketData->fUserId = db.getInt("f_user");
        db[":f_iplogout"] = fPeerAddress;
        db[":f_dateend"] = QDate::currentDate();
        db[":f_timeend"] = QTime::currentTime();
        db.update("s_login_session", where_id(recid));
    } else {
        fResponseCode = dr_login_failed;
        return false;
    }

    db[":f_id"] = fSocketData->fUserId;
    db.exec("select f_id, f_group, f_first, f_last, f_config from s_user where f_id=:f_id");
    if (db.nextRow()) {
        fData.clear();
        jo = QJsonObject();
        jo["id"] = db.getInt("f_id");
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

void SocketHandlerLoginWithSession::processData()
{

}
