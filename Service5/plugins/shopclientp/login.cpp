#include "login.h"
#include "ops.h"
#include "translator.h"

int loginPasswordHash(RawMessage &rm, Database &db, const QByteArray &in, QString &username)
{
    QString passhash, firebaseToken;
    rm.readString(passhash, in);
    rm.readString(firebaseToken, in);
    db[":f_passhash"] = passhash;
    int userid = 0;

    if (db.exec("select f_id, concat_ws(' ', f_last, f_first) as fullname from s_user where f_passhash=:f_passhash") == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return 0;
    }
    if (db.next()) {
        rm.putUByte(1);
        username = db.string("fullname");
        userid = db.integer("f_id");
        db[":f_id"] = db.integer("f_id");
        db[":f_firebase_token"] = firebaseToken;
        if (!db.update("s_user", "f_id", userid)) {
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

    return 0;
}

int loginPin(RawMessage &rm, Database &db, const QByteArray &in, QString &username)
{
    QString pin, passhash, firebaseToken;
    int userid = 0;
    rm.readString(pin, in);
    rm.readString(firebaseToken, in);
    db[":f_altpassword"] = QString::fromLatin1(QCryptographicHash::hash(pin.toUtf8(), QCryptographicHash::Md5).toHex());
    if (db.exec("select f_id, concat_ws(' ', f_last, f_first) as fullname from s_user where f_altpassword=:f_altpassword ") == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return 0;
    }
    if (db.next()) {
        userid = db.integer("f_id");
        passhash = db.uuid();
        username = db.string("fullname");
        db[":f_passhash"] = passhash;
        db[":f_firebase_token"] = firebaseToken;
        if (!db.update("s_user", "f_id", userid)) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return 0;
        }
    } else {
        rm.putUByte(0);
        rm.putString(ntr("Username or password incorrect", ntr_am));
        return 0;
    }
    rm.putUByte(1);
    rm.putString(passhash);
    rm.putString(username);
    return userid;
}
