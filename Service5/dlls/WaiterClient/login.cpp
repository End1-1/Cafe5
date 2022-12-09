#include "login.h"
#include "translator.h"
#include "ops.h"

void loginUsernamePassword(RawMessage &rm, Database &db, const QByteArray &in)
{
    quint8 version;
    QString username, password;
    rm.readUByte(version, in);
    rm.readString(username, in);
    rm.readString(password, in);
    if (username.isEmpty() || password.isEmpty()) {
        rm.putUByte(0);
        rm.putString(tr("Username or password incorrect"));
        return;
    }

    int userid;
    QString fullname, passhash;
    db[":user"] = username;
    db[":pass"] = password;
    switch (version) {
    case version1:
        rm.putUByte(0);
        rm.putString(tr("Not implemented"));
        break;
    case version2:
        rm.putUByte(0);
        rm.putString(tr("Not implemented"));
        break;
    case version3:
        if (db.exec("select f_id, f_firstname, f_lastname from users where lower(f_username)=:user and f_password=md5(:pass)") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (db.next()) {
            userid = db.integer("f_id");
            passhash = db.uuid();
            fullname = QString("%1 %2").arg(db.string("f_firstname"), db.string("f_lastname"));
            db[":f_passhash"] = passhash;
            db.update("users", "f_id", userid);
        } else {
            rm.putUByte(0);
            rm.putString(tr("Username or password incorrect"));
            return;
        }
        break;
    }
    rm.putUByte(1);
    rm.putString(passhash);
    rm.putString(fullname);
}

void loginPasswordHash(RawMessage &rm, Database &db, const QByteArray &in)
{
    quint8 version;
    QString passhash;
    rm.readUByte(version, in);
    rm.readString(passhash, in);
    db[":f_passhash"] = passhash;
    switch (version) {
    case version1:
        rm.putUByte(0);
        rm.putString(tr("Not implemented"));
        break;
    case version2:
        rm.putUByte(0);
        rm.putString(tr("Not implemented"));
        break;
    case version3:
        if (db.exec("select f_id, f_firstname, f_lastname from users where f_passhash=:f_passhash") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (db.next()) {
            rm.putUByte(1);
            return;
        } else {
            rm.putUByte(0);
            rm.putString(tr("Username or password incorrect"));
            return;
        }
        break;
    }
}
