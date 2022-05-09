#include "standard.h"
#include "requesthandler.h"
#include "jsonhandler.h"
#include "database.h"
#include "logwriter.h"
#include "commandline.h"
#include "gtranslator.h"
#include <QJsonObject>
#include <QSettings>
#include <QJsonArray>
#include <QRandomGenerator>
#include <QUuid>
#include <QDir>

enum RegistrationState {rsNew = 1, rsConfirmed};

void routes(QStringList &r)
{
    r.append("notfound");
    r.append("authorization");
    r.append("registration");
    r.append("confirmregistration");
    r.append("authorization");
    r.append("chat");
}

bool notfound(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType)
{
    Q_UNUSED(dataMap);
    RequestHandler rh(outdata);
    rh.fContentType = contentType;
    rh.setResponse(HTTP_NOT_FOUND, QString("%1 not found on this server").arg(indata.data()));
    return true;
}

bool authorization(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType)
{
    //VALIDATION
    RequestHandler rh(outdata);
    rh.fContentType = contentType;
    QString message;
    if ((!dataMap.contains("phone") && !dataMap.contains("email"))) {
        message.append("Phone or email required.");
    }
    if (!dataMap.contains("password")) {
        message.append("Password required.");
    }
    if (!message.isEmpty()) {
        JsonHandler jh;
        jh["message"] = message;
        rh.setResponse(HTTP_DATA_VALIDATION_ERROR, jh.toString());
        return false;
    }

    //PROCESS
    JsonHandler jh;
    Database db;
    if (!db.open("./config.ini")) {
        jh["message"] = "Cannot connect to database";
        return rh.setInternalServerError(jh.toString());
    }

    db[":fphone"] = getData(indata, dataMap["phone"]);
    db[":femail"] = getData(indata, dataMap["email"]);
    db[":fpassword"] = getData(indata, dataMap["password"]);
    db.exec("select uuid() as session, u.* from users_list u where (u.femail=:femail or u.fphone=:fphone) and u.fpassword=:fpassword");
    if (db.next()) {
        jh["uuid"] = db("uuid");
        jh["reply"] = 1;
        rh.setResponse(HTTP_OK, jh.toString());
        return true;
    } else {
        jh["reply"] = 0;
        jh["message"] = "Authentication failed.";
        rh.setResponse(HTTP_FORBIDDEN, jh.toString());
        return false;
    }
}

bool chat(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType)
{
    //VALIDATION
    RequestHandler rh(outdata);
    rh.fContentType = contentType;
    QString err;
    QString authMethod;
    QString user;
    QString password;
    QString action;
    if (!dataMap.contains("auth")) {
        err += "Authentication method missing";
    } else {
        QStringList authMethods;
        authMethods.append("up");
        for (const QString &am: authMethods) {
            if (getData(indata, dataMap["auth"]) == am) {
                authMethod = am;
                break;
            }
        }
        if (authMethod.isEmpty()) {
            err += "Valid authentication method missing. ";
        } else {
            if (authMethod == "up") {
                if (!dataMap.contains("user")) {
                    err += "Username reqired. ";
                } else {
                    user = getData(indata, dataMap["user"]);
                }
                if (!dataMap.contains("pass")) {
                    err += "Password required";
                } else {
                    password = getData(indata, dataMap["pass"]);
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
            if (getData(indata, dataMap["a"]).toLower() == a) {
                action = a;
                break;
            }
        }
        if (action.isEmpty()) {
            err += "Valid action required. ";
        }
    }
    if (err.isEmpty() == false) {
        rh.setDataValidationError("Standard::Chat: " + err);
        return false;
    }

    //PROCESS
    JsonHandler jh;
    Database db;
    QString configFile;
    CommandLine cl;
    if (!cl.value("--config", configFile)) {
        jh["reply"] = 0;
        jh["message"] = "Standard::Chat: No config file.";
        rh.setForbiddenError(jh.toString());
        return false;
    }
    if (!db.open(configFile)) {
        jh["reply"] = 0;
        jh["message"] = "Standard::Chat: cannot connect to database";
        return rh.setInternalServerError(jh.toString());
    }
    db[":fphone"] = user;;
    db[":femail"] = user;
    db[":fpassword"] = password;
    db.exec("select uuid() as session, u.* from users_list u where (u.femail=:femail or u.fphone=:fphone) and u.fpassword=md5(:fpassword)");
    if (db.next() == false) {
        jh["reply"] = 0;
        jh["message"] = "Authentication failed.";
        rh.setForbiddenError(jh.toString());
        return false;
    }
    int userid = db.integerValue("fid");
    if (action == "get") {
        QJsonArray ja;
        db[":freceiver"] = userid;
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
    } else if (action == "read") {
        db.exec(QString("update users_chat set fstate=3, fdateread=current_timestamp where fid in (%1)").arg(QString(getData(indata, dataMap["list"]))));
    } else if (action == "post") {
        int recipient = getData(indata, dataMap["recipient"]).toInt();
        if (recipient == 0) {
            return rh.setDataValidationError("Recipent not valid.");
        }
        if (getData(indata, dataMap["usermap"]).toInt() == 1) {
            db[":fid"] = recipient;
            db.exec("select fuser from users_store where fid=:fid");
            if (!db.next()) {
                return rh.setInternalServerError("No such user in the map");
            }
            recipient = db.integerValue("fuser");
        }
        db[":fdateserver"] = QDateTime::currentDateTime();
        db[":fstate"] = 1;
        db[":fsender"] = userid;
        db[":freceiver"] = recipient;
        db[":fmessage"] = getData(indata, dataMap["message"]);
        int msgid = 0;
        if (db.insert("users_chat", msgid)) {
            jh["messageid"] = msgid;
        } else {
            jh["msg"] = db.lastDbError();
            return rh.setInternalServerError(jh.toString());
        }
    }
    return rh.setResponse(HTTP_OK, jh.toString());
}

bool registration(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType)
{
    //VALIDATION
    RequestHandler rh(outdata);
    rh.fContentType = contentType;
    QString message;
    if ((!dataMap.contains("phone") && !dataMap.contains("email"))) {
        message += "Phone or email required.";
    }
    if (!dataMap.contains("password")) {
        message += "Password required.";
    }
    if (!message.isEmpty()) {
        JsonHandler jh;
        jh["message"] = message;
        return rh.setDataValidationError(jh.toString());
    }
    //PROCESS
    JsonHandler jh;
    Database db;
    if (!db.open("./config.ini")) {
        jh["message"] = "Cannot connect to database";
        return rh.setInternalServerError(jh.toString());
    }

    db[":fphone"] = getData(indata, dataMap["phone"]);
    db[":femail"] = getData(indata, dataMap["email"]);
    db.exec("select fid from users_list where (length(femail)>0 and femail=:femail) || (length(fphone)>0 and fphone=:fphone)");
    if (db.next()) {
        jh["message"] = "An account exists with given data. You should use authentication or password recover";
        rh.fHttpHeader.setLocation("/authentication");
        return rh.setResponse(HTTP_SEE_OTHER, jh.toString());
    }
    const quint32 low = 100000;
    const quint32 high = 999999;
    QRandomGenerator rg = QRandomGenerator::securelySeeded();
    QString token = QUuid::createUuid().toString() + "-" + QUuid::createUuid().toString();
    token.replace("{", "").replace("}", "");
    db[":fstate"] = RegistrationState::rsNew;
    db[":fphone"] = getData(indata, dataMap["phone"]);
    db[":femail"] = getData(indata, dataMap["email"]);
    db[":fpassword"] = getData(indata, dataMap["password"]);
    db[":fconfirmation_code"] = rg.bounded(low, high);
    db[":ftoken"] = token;
    db[":fcreatedate"] = QDate::currentDate();
    db[":fcreatetime"] = QTime::currentTime();
    db.insert("users_registration");
    if (!getData(indata, dataMap["phone"]).isEmpty()) {
        jh["message"] = "A confirmation code was sent to your phone.";
    }
    if (!getData(indata, dataMap["email"]).isEmpty()) {
        jh["message"] = "A confirmation code was sent to your email.";
    }
    jh["token"] = token;
    return rh.setResponse(HTTP_OK, jh.toString());
}

bool confirmregistration(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType)
{
    //VALIDATION
    RequestHandler rh(outdata);
    rh.fContentType = contentType;
    QString message;
    JsonHandler jh;
    if (!dataMap.contains("token")) {
        message += "Missing token.";
    }
    if (!dataMap.contains("code")) {
        message += "Missing code.";
    }
    if (!message.isEmpty()) {
        jh["message"] = message;
        return rh.setDataValidationError(jh.toString());
    }
    //PROCESS
    Database db;
    if (!db.open("./config.ini")) {
        jh["message"] = "Cannot connect to database";
        return rh.setInternalServerError(jh.toString());
    }
    db[":ftoken"] = getData(indata, dataMap["token"]);
    db[":fconfirmation_code"] = getData(indata, dataMap["code"]);
    db[":fstate"] = RegistrationState::rsNew;
    db.exec("select * from users_registration where fconfirmation_code=:fconfirmation_code and ftoken=:ftoken and fstate=:fstate");
    if (!db.next()) {
        return rh.setDataValidationError("Invalid code");
    }
    int id = db("fid").toInt();
    int confirmid = 0;
    db[":fgroup"] = 2;
    db[":fphone"] = db("fphone");
    db[":femail"] = db("femail");
    db[":fpassword"] = db("fpassword");
    db[":ffirstname"] = "";
    db[":flastname"] = "";
    if (!db.insert("users_list", confirmid)) {
        return rh.setInternalServerError(db.lastDbError());
    }
    db[":fid"] = id;
    db[":fstate"] = RegistrationState::rsConfirmed;
    db[":fconfirmdate"] = QDate::currentDate();
    db[":fconfirmtime"] = QTime::currentTime();
    db[":fconfirmid"] = confirmid;
    db.update("users_registration");
    jh["message"] = "ok";
    return rh.setResponse(HTTP_OK, jh.toString());
}
