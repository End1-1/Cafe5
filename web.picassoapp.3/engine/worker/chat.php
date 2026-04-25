<?php
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
    int userid = db.integer("fid");
    if (action == "get") {
        QJsonArray ja;
        db[":freceiver"] = userid;
        db[":fstate"] = 1;
        db.exec("select fid, fdateserver, fsender, fmessage from users_chat where fstate=:fstate and freceiver=:freceiver order by fdateserver");
        QStringList ids;
        while (db.next()) {
            QJsonObject jo;
            jo["id"] = db.integer("fid");
            jo["msgdate"] = db.dateTimeValue("fdateserver").toString("dd/MM/yyyy HH:mm");
            jo["sender"] = db.integer("fsender");
            jo["message"] = db.string("fmessage");
            ja.append(jo);
            ids.append(db.string("fid"));
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
            recipient = db.integer("fuser");
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