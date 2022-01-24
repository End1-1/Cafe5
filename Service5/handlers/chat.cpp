#include "chat.h"
#include "dduser.h"
#include "jsonhandler.h"
#include "databaseconnectionmanager.h"
#include <QJsonArray>

Chat::Chat() :
    RequestHandler()
{

}

bool Chat::validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    QString err;
    if (!dataMap.contains("auth")) {
        err += "Authentication method missing";
    } else {
        QStringList authMethods;
        authMethods.append("up");
        for (const QString &am: authMethods) {
            if (getData(data, dataMap["auth"]) == am) {
                fAuthMethod = am;
                break;
            }
        }
        if (fAuthMethod.isEmpty()) {
            err += "Valid authentication method missing. ";
        } else {
            if (fAuthMethod == "up") {
                if (!dataMap.contains("user")) {
                    err += "Username reqired. ";
                } else {
                    fUser = getData(data, dataMap["user"]);
                }
                if (!dataMap.contains("pass")) {
                    err += "Password required";
                } else {
                    fPassword = getData(data, dataMap["pass"]);
                }
            }
        }
    }
    if (!dataMap.contains("a")) {
        err += "Action required. ";
    } else {
        QStringList actions;
        actions.append("get");
        actions.append("read");
        actions.append("post");
        for (const QString &a: actions) {
            if (getData(data, dataMap["a"]).toLower() == a) {
                fAction = a;
                break;
            }
        }
        if (fAction.isEmpty()) {
            err += "Valid action required. ";
        }
    }
    if (!err.isEmpty()) {
        return setDataValidationError(err);
    }
    return true;
}

bool Chat::handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    Database db;
    JsonHandler jh;
    if (!DatabaseConnectionManager::openDatabase(db, jh)) {
        return setInternalServerError(jh.toString());
    }
    DDUser u;
    if (!u.authUserPass(db, fUser, "", fPassword)) {
        return setForbiddenError("Access denied");
    }
    if (fAction == "get") {
        QJsonArray ja;
        db[":freceiver"] = u.fId;
        db[":fstate"] = 1;
        db.exec("select fid, fdateserver, fsender, fmessage from users_chat where fstate=:fstate and freceiver=:freceiver order by fdateserver");
        QStringList ids;
        while (db.next()) {
            QJsonObject jo;
            jo["id"] = db.integerValue("fid");
            jo["msgdate"] = db.dateTimeValue("fdateserver").toString("dd/MM/yyyy HH:mm");
            jo["sender"] = db.integerValue("fsender");
            jo["message"] = db.stringValue("fmessage");
            ja.append(jo);
            ids.append(db.stringValue("fid"));
        }
        if (ids.count() > 0) {
            jh["messages"] = ja;
            jh["idlist"] = ids.join(",");
            db.exec(QString("update users_chat set fstate=2, fdatereceive=current_timestamp where fid in (%1)").arg(ids.join(",")));
        }
    } else if (fAction == "read") {
        db.exec(QString("update users_chat set fstate=3, fdateread=current_timestamp where fid in (%1)").arg(QString(getData(data, dataMap["list"]))));
    } else if (fAction == "post") {
        int recipient = getData(data, dataMap["recipient"]).toInt();
        if (recipient == 0) {
            return setDataValidationError("Recipent not valid.");
        }
        if (getData(data, dataMap["usermap"]).toInt() == 1) {
            db[":fid"] = recipient;
            db.exec("select fuser from users_store where fid=:fid");
            if (!db.next()) {
                return setInternalServerError("No such user in the map");
            }
            recipient = db.integerValue("fuser");
        }
        db[":fdateserver"] = QDateTime::currentDateTime();
        db[":fstate"] = 1;
        db[":fsender"] = u.fId;
        db[":freceiver"] = recipient;
        db[":fmessage"] = getData(data, dataMap["message"]);
        int msgid = 0;
        if (db.insert("users_chat", msgid)) {
            jh["messageid"] = msgid;
        } else {
            jh["msg"] = db.lastDbError();
            return setInternalServerError(jh.toString());
        }
    }
    return setResponse(HTTP_OK, jh.toString());
}
