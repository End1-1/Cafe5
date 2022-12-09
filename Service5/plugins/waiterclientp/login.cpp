#include "login.h"
#include "ops.h"
#include "c5translator.h"

int loginUsernamePassword(RawMessage &rm, Database &db, const QByteArray &in, QString &fullname)
{
    quint8 version;
    QString username, password;
    rm.readUByte(version, in);
    rm.readString(username, in);
    rm.readString(password, in);
    if (username.isEmpty() || password.isEmpty()) {
        rm.putUByte(0);
        rm.putString(ntr("Username or password incorrect", ntr_am));
        return 0;
    }

    int userid = 0;
    QString passhash;
    db[":user"] = username;
    db[":pass"] = password;
    switch (version) {
    case version1:
        rm.putUByte(0);
        rm.putString(ntr("Not implemented", ntr_am));
        break;
    case version2:
        rm.putUByte(0);
        rm.putString(ntr("Not implemented", ntr_am));
        break;
    case version3:
        if (db.exec("select f_id, concat(f_lastname, ' ', f_firstname) as fullname from users where lower(f_username)=:user and f_password=md5(:pass)") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return 0;
        }
        if (db.next()) {
            userid = db.integer("f_id");
            passhash = db.uuid();
            fullname = db.string("fullname");
            db[":f_passhash"] = passhash;
            db.update("users", "f_id", userid);
        } else {
            rm.putUByte(0);
            rm.putString(ntr("Username or password incorrect", ntr_am));
            return 0;
        }
        break;
    }
    rm.putUByte(1);
    rm.putString(passhash);
    rm.putString(fullname);
    return userid;
}

int loginPasswordHash(RawMessage &rm, Database &db, const QByteArray &in, QString &username)
{
    quint8 version;
    QString passhash, firebaseToken;
    rm.readUByte(version, in);
    rm.readString(passhash, in);
    rm.readString(firebaseToken, in);
    db[":f_passhash"] = passhash;
    int userid = 0;
    switch (version) {
    case version1:
        if (db.exec("select id, lname || ' ' || fname as fullname from employes where passhash=:f_passhash") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return 0;
        }
        if (db.next()) {
            rm.putUByte(1);
            username = db.string("fullname");
            userid = db.integer("id");
            db[":id"] = db.integer("id");
            db[":firebase_token"] = firebaseToken;
            if (!db.update("employes", "id", userid)) {
                rm.putUByte(0);
                rm.putString(db.lastDbError());
                return 0;
            }
            return userid;
        } else {
            rm.putUByte(0);
            rm.putString(ntr("Username or password incorrect", ntr_am));
            return 0;
        }
        break;
    case version2:
        rm.putUByte(0);
        rm.putString(ntr("Not implemented", ntr_am));
        break;
    case version3:
        if (db.exec("select f_id, concat(f_lastname, ' ', f_firstname) as fullname from users where f_passhash=:f_passhash") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return 0;
        }
        if (db.next()) {
            rm.putUByte(1);
            username = db.string("fullname");
            return db.integer("f_id");
        } else {
            rm.putUByte(0);
            rm.putString(ntr("Username or password incorrect", ntr_am));
            return 0;
        }
        break;
    }
    return 0;
}

int loginPin(RawMessage &rm, Database &db, const QByteArray &in, QString &username)
{
    quint8 version;
    QString pin, passhash, firebaseToken;
    int userid = 0;
    rm.readUByte(version, in);
    rm.readString(pin, in);
    rm.readString(firebaseToken, in);
    db[":f_pin"] = pin;
    switch (version) {
    case version1:
        db[":f_pin"] = QString::fromLatin1(QCryptographicHash::hash(pin.toUtf8(), QCryptographicHash::Md5).toHex());
        if (db.exec("select id, lname || ' ' || fname as fullname from employes where password2=:f_pin ") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return 0;
        }
        if (db.next()) {
            userid = db.integer("id");
            passhash = db.uuid();
            username =  db.string("fullname");
            db[":passhash"] = passhash;
            db[":firebase_token"] = firebaseToken;
            if (!db.update("employes", "id", userid)) {
                rm.putUByte(0);
                rm.putString(db.lastDbError());
                return 0;
            }
        } else {
            rm.putUByte(0);
            rm.putString(ntr("Username or password incorrect", ntr_am));
            return 0;
        }
        break;
    case version2:
        rm.putUByte(0);
        rm.putString(ntr("Not implemented", ntr_am));
        return 0;
    case version3:
        if (db.exec("select f_id, concat(f_lastname, ' ', f_firstname) as fullname from users where f_altpassword=md5(:f_pin)") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return 0;
        }
        if (db.next()) {
            userid = db.integer("f_id");
            passhash = db.uuid();
            username = db.string("fullname");
            db[":f_passhash"] = passhash;
            db.update("users", "f_id", userid);
        } else {
            rm.putUByte(0);
            rm.putString(ntr("Username or password incorrect", ntr_am));
            return 0;
        }
        break;
    }
    rm.putUByte(1);
    rm.putString(passhash);
    rm.putString(username);
    return userid;
}
