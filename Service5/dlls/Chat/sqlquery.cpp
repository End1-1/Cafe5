#include "sqlquery.h"
#include "logwriter.h"
#include "commandline.h"
#include "jsons.h"
#include "database.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QSettings>

bool newmessage(const QJsonObject &jreq, QJsonObject &jret, QString &err)
{
    QString path;
    Database db;
    if (!db.open("127.0.0.1", jreq["database"].toString(), "root", "root5")) {
        err = db.lastDbError();
        return false;
    }
    db[":f_session"] = jreq["sessionkey"].toString();
    db.exec("select * from s_login_session where f_session=:f_session");
    if (db.next() == false) {
        err = "Xaker-naker?";
        return false;
    }
    int userfrom = db.integer("f_user");
    db[":f_state"] = 1;
    db[":f_userfrom"] = userfrom;
    db[":f_userto"] = jreq["to"].toInt();
    db[":f_created"] = QDateTime::currentDateTime();
    db[":f_body"] = __jsonstr(jreq["message"].toObject());
    if (!db.insert("sys_chat")) {
        err = db.lastDbError();
        return false;
    }
    return true;
}

bool getunread(const QJsonObject &jreq, QJsonObject &jret, QString &err)
{
    CommandLine cl;
    QString path;
    Database db;
    if (!db.open("127.0.0.1", jreq["database"].toString(), "root", "root5")) {
        err = db.lastDbError();
        return false;
    }
    db[":f_session"] = jreq["sessionkey"].toString();
    if (!db.exec("select * from s_login_session where f_session=:f_session")) {
        err = db.lastDbError();
        return false;
    }
    if (db.next() == false) {
        err = "Xaker-naker?";
        return false;
    }
    int userfrom = db.integer("f_user");
    db[":f_state"] = 1;
    db[":f_userto"] = userfrom;
    if (!db.exec("select c.f_id, c.f_created, c.f_body, "
                 "u.f_login, u.f_last, u.f_first "
                 "from sys_chat c "
                 "left join s_user u on u.f_id=c.f_userfrom "
                 "where c.f_state=:f_state and c.f_userto=:f_userto")) {
        err = db.lastDbError();
        return false;
    }
    QJsonArray jm;
    while (db.next()) {
        jm.append(QJsonObject{{"f_id", db.integer("f_id")},
            {"f_created", db.string("f_created")},
            {"f_login", db.string("f_login")},
            {"f_last", db.string("f_last")},
            {"f_first", db.string("f_first")},
            {"f_body", __strjson(db.string("f_body"))}});
    }
    for (int i = 0; i < jm.size(); i++) {
        const QJsonObject &jmo = jm.at(i).toObject();
        db[":f_id"] = jmo["f_id"].toInt();
        db.exec("update sys_chat set f_state=2, f_readed=current_timestamp where f_id=:f_id");
    }
    jret["chat"] = jm;
    return true;
}

bool clearbasket(const QJsonObject &jreq, QJsonObject &jret, QString &err)
{
    CommandLine cl;
    QString path;
    Database db;
    if (!db.open("127.0.0.1", jreq["database"].toString(), "root", "root5")) {
        err = db.lastDbError();
        return false;
    }
    db[":f_session"] = jreq["sessionkey"].toString();
    if (!db.exec("select * from s_login_session where f_session=:f_session")) {
        err = db.lastDbError();
        return false;
    }
    if (db.next() == false) {
        err = "Xaker-naker?";
        return false;
    }
    db.exec("select * from s_app where f_app='MENU'");
    db.next();
    int v = db.string("f_version").toInt() + 1;
    db[":f_version"] = v;
    db.exec("update s_app set f_version=:f_version where f_app='MENU'");
    return true;
}
