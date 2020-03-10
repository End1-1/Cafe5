#include "sockethandlerloginwithsession.h"
#include "c5database.h"
#include "config.h"
#include "servicecommands.h"
#include <QJsonDocument>
#include <QJsonObject>

SocketHandlerLoginWithSession::SocketHandlerLoginWithSession(QByteArray &data) :
    SocketHandler(data)
{

}

void SocketHandlerLoginWithSession::processData()
{
    QJsonDocument jdoc = QJsonDocument::fromJson(fData);
    QJsonObject jo = jdoc.object();
    QByteArray session =  QByteArray::fromBase64(jo["session"].toString().toUtf8());
    C5Database db(DBHOST, DBFILE, DBUSER, DBPASSWORD);
    db[":f_session"] = session;
    db.exec("select f_id, f_user from s_login_session where f_session=:f_session");
    int userid = 0;
    if (db.nextRow()) {
        int recid = db.getInt("f_id");
        userid = db.getInt("f_user");
        db[":f_iplogout"] = fPeerAddress;
        db[":f_dateend"] = QDate::currentDate();
        db[":f_timeend"] = QTime::currentTime();
        db.update("s_login_session", where_id(recid));
    } else {
        fResponseCode = dr_login_failed;
        return;
    }

    db[":f_id"] = userid;
    db.exec("select f_id, f_group, f_first, f_last from s_user where f_id=:f_id");
    if (db.nextRow()) {
        fData.clear();
        jo = QJsonObject();
        jo["id"] = db.getInt("f_id");
        jo["group"] = db.getInt("f_group");
        jo["firstname"] = db.getString("f_first");
        jo["lastname"] = db.getString("f_last");
        QByteArray session = db.uuid_bin();
        jo["session"] = QString(session.toBase64());
        db[":f_session"] = session;
        db[":f_user"] = jo["id"].toInt();
        db[":f_iplogin"] = fPeerAddress;
        db[":f_datestart"] = QDate::currentDate();
        db[":f_timestart"] = QTime::currentTime();
        db.insert("s_login_session", false);
        jdoc = QJsonDocument(jo);
        fData = jdoc.toJson(QJsonDocument::Compact);
    } else {
        fResponseCode = dr_login_failed;
    }
}
