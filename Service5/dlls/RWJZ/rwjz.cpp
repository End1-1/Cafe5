#include "rwjz.h"
#include "commandline.h"
#include "database.h"
#include "logwriter.h"
#include "gtranslator.h"
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#define op_get_list 3
#define op_update_state 4
#define op_remove_dish 5

bool getList(quint8 op, RawMessage &rm, Database &db, const QByteArray &in) {
    rm.putUByte(op);
    QString state = "0,1,2";
    quint32 reminder;
    rm.readUInt(reminder, in);

    QString sql = QString("select r.record_id,"
        "lpad(extract(hour from cast(r.date_register as time)),2,'0') || ':' || lpad(extract(minute from cast(r.date_register as time)),2,'0') as reg_time, "
        "t.name as table_name, r.state_id, "
        "e.lname || ' ' || e.fname as staff_name, d.name as dish_name, r.qty, "
        "od.comments "
        "from o_dishes_reminder r "
        "inner join h_table t on t.id=r.table_id "
        "inner join employes e on e.id=r.staff_id "
        "inner join me_dishes d on d.id=r.dish_id "
        "inner join o_dishes od on od.id=r.record_id "
        "where r.state_id in (%1) and r.reminder_id=%2 "
        "order by r.id ").arg(state, QString::number(reminder));
    if (db.exec(sql) == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return false;
    }

    QStringList state1;
    QJsonArray ja;
    while (db.next()) {
        QJsonObject jo;
        jo["rec"] = db.integer("record_id");
        jo["time"] = db.string("reg_time");
        jo["table"] = db.string("table_name");
        jo["staff"] = db.string("staff_name");
        jo["qty"] = db.string("qty");
        jo["state"] = db.integer("state_id");
        jo["dish"] = db.string("dish_name");
        jo["comment"] = db.string("comments");
        ja.append(jo);
        if (db.integer("state_id") == 0) {
            state1.append(db.string("record_id"));
        }
    }
    if (state1.count() > 0) {
        db.exec(QString("update o_dishes_reminder set state_id=1 where record_id in (%1)").arg(state1.join(",")));
    }
    QJsonDocument jd(ja);
    rm.putUByte(1);
    rm.putString(jd.toJson(QJsonDocument::Compact));
    return true;
}

bool updateState(quint8 op, RawMessage &rm, Database &db, const QByteArray &in) {
    rm.putUByte(op);
    quint32 recid;
    quint8 state;
    rm.readUInt(recid, in);
    rm.readUByte(state, in);

    db[":record_id"] = recid;
    db.exec("select state_id from o_dishes_reminder where record_id=:record_id");
    if (db.next() == false) {
        rm.putUByte(0);
        rm.putString("Invalid record id");
        return false;
    }
    int currstate = db.integer("state_id");
    db[":state_id"] = state;
    switch (state) {
    case 2:
        db[":date_start"] = QDateTime::currentDateTime();
        break;
    case 3:
        if (currstate == 1) {
            db[":date_start"] = QDateTime::currentDateTime();
        }
        db[":date_ready"] = QDateTime::currentDateTime();
        break;
    case 5:
        db[":date_removed"] = QDateTime::currentDateTime();
        break;
    }
    if (db.update("o_dishes_reminder", "record_id", recid) == false) {
        rm.putUByte(0);
        rm.putString(db.lastDbError());
        return false;
    }

    rm.putUByte(1);
    rm.putUInt(recid);
    rm.putUByte(state);
    return true;
}

bool dllfunc(const QByteArray &in, RawMessage &rm) {
    QString dbname;
    rm.readString(dbname, in);
    quint8 op;
    rm.readUByte(op, in);

    CommandLine cl;
    QString configFile;
    cl.value("--config", configFile);
    QSettings s(configFile, QSettings::IniFormat);
    Database db;
    if (db.open(s.value("db/host").toString(), s.value("db/schema").toString(), s.value("db/username").toString(), s.value("db/password").toString()) == false) {
        rm.putUByte(2);
        rm.putString(db.lastDbError());
        LogWriter::write(LogWriterLevel::errors, rm.property("session").toString(), "#1. " + db.lastDbError());
        LogWriter::write(LogWriterLevel::errors, rm.property("session").toString(), "Config file: " + configFile);
        return false;
    }
    db[":fname"] = dbname;
    db.exec("select * from system_databases where fname=:fname");
    if (db.next() == false) {
        rm.putUByte(2);
        rm.putString("Unknown database name");
        return false;
    }
    Database dbw("QIBASE");
    if (dbw.open(db.string("fhost"), db.string("fschema"), db.string("fusername"), db.string("fpassword")) == false) {
        rm.putUByte(2);
        rm.putString("#2. " + dbw.lastDbError());
        return false;
    }

    switch (op) {
    case op_get_list:
        return getList(op, rm, dbw, in);
    case op_update_state:
        return updateState(op, rm, dbw, in);
    default:
        rm.putUByte(2);
        rm.putString(gr("Function code unknown.") + "\r\n" + QString::number(op));
        return false;
    }
    return true;
}
