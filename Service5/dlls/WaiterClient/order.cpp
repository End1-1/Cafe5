#include "order.h"
#include "translator.h"
#include "networktable.h"
#include "ops.h"

void openTable(RawMessage &rm, Database &db, const QByteArray &in)
{
    quint8 version;
    quint32 tableid;
    QString sessionid;
    rm.readUByte(version, in);
    rm.readUInt(tableid, in);
    rm.readString(sessionid, in);

    if (sessionid.isEmpty()) {
        rm.putUByte(0);
        rm.putString(tr("Empty session id"));
        return;
    }

    //check locked
    db[":f_id"] = tableid;
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
        if (db.exec("select f_lockhost from r_table where f_id=:f_id") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (db.next()) {
            if (db.string("f_lockhost").isEmpty() == false) {
                if (db.string("f_lockhost") != sessionid) {
                    rm.putUByte(0);
                    rm.putString(tr("Table already editing by other user"));
                    return;
                }
            }
            db[":f_lockhost"] = sessionid;
            db[":f_id"] = tableid;
            db.exec("update r_table set f_lockhost=:f_lockhost, f_locktime=unix_timestamp(now()) where f_id=:f_id");
        } else {
            rm.putUByte(0);
            rm.putString(tr("Wrong table id"));
            return;
        }
        break;
    }

    rm.putUByte(1);
}

void unlockTable(RawMessage &rm, Database &db, const QByteArray &in)
{
    quint8 version;
    quint32 tableid;
    QString sessionid;
    rm.readUByte(version, in);
    rm.readUInt(tableid, in);
    rm.readString(sessionid, in);

    if (sessionid.isEmpty()) {
        rm.putUByte(0);
        rm.putString(tr("Empty session id"));
        return;
    }


    db[":f_id"] = tableid;
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
        if (db.exec("select f_lockhost from r_table where f_id=:f_id") == false) {
            rm.putUByte(0);
            rm.putString(db.lastDbError());
            return;
        }
        if (db.next()) {
            if (db.string("f_lockhost").isEmpty() == false) {
                if (db.string("f_lockhost").toLower() != sessionid.toLower()) {
                    rm.putUByte(0);
                    rm.putString(tr("You cannot unlock host thats not locked by you"));
                    return;
                }
            }
            db[":f_id"] = tableid;
            db.exec("update r_table set f_lockhost='', f_locktime=0 where f_id=:f_id");
        } else {
            rm.putUByte(0);
            rm.putString(tr("Username or password incorrect"));
            return;
        }
        break;
    }

    rm.putUByte(1);
}

void createHeaderOfOrder(RawMessage &rm, Database &db, const QByteArray &in)
{
    quint8 version;
    quint32 tableid;
    QString sessionid;
    rm.readUByte(version, in);
    rm.readUInt(tableid, in);
    rm.readString(sessionid, in);
}
