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
        db.exec("select fid, fsender, fmessage from users_chat where fstate=:fstate and freceiver=:freceiver order by fdateserver");
        QStringList ids;
        while (db.next()) {
            QJsonObject jo;
            jo["id"] = db.integerValue("fid");
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
    }
    return setResponse(HTTP_OK, jh.toString());
}
