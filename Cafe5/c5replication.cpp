#include "c5replication.h"
#include "c5config.h"
#include "c5utils.h"
#include "c5shoporder.h"
#include <QThread>

C5Replication::C5Replication() :
    QObject()
{
    fIgnoreErrors = false;
}

void C5Replication::start(const char *slot)
{
    QThread *t = new QThread();
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
    connect(t, SIGNAL(finished()), this, SLOT(deleteLater()));
    connect(t, SIGNAL(started()), this, slot);
    connect(this, SIGNAL(finished()), t, SLOT(quit()));
    moveToThread(t);
    t->start();
}

void C5Replication::downloadFromServer()
{
    downloadDataFromServer(__c5config.replicaDbParams(), __c5config.dbParams());
}

bool C5Replication::ret(C5Database &db1, C5Database &db2)
{
    db1.rollback();
    db2.rollback();
    emit finished();
    return true;
}

void C5Replication::downloadDataFromServer(const QStringList &src, const QStringList &dst)
{
    int steps = 0;
    int step = 1;
    bool hc = false;
    C5Database dr(src);
    C5Database dr2(src);
    emit progress("Connecting to replication database");
    if (!dr.open()) {
        emit progress("Connection to " + src.at(0) + ":" + src.at(1) + " failed.<br>" + dr.fLastError);
        emit finished();
        return;
    }
    C5Database db(dst);
    db[":f_host"] = db.fDb.hostName();
    db[":f_db"] = db.fDb.databaseName();
    db.exec("select f_syncin from s_db where f_host=:f_host and f_db=:f_db");
    if (!db.nextRow()) {
        emit progress("Cannot get syncronization number");
        emit finished();
        return;
    }
    int lastid = db.getInt(0);
    QStringList tl;
    dr.exec("select * from s_syncronize_in order by f_id");
     while (dr.nextRow()) {
        tl.append(dr.getString("f_table"));
    }
    steps = 2;
    if (lastid == 0) {
        emit progress("Initializing");
        steps = tl.count() + 2;
        dr.exec("select max(f_id) from s_syncronize");
        dr.nextRow();
        lastid = dr.getInt(0);
        db.startTransaction();
        foreach (const QString &tn, tl) {
            db.exec("delete from " + tn);
        }
        for (int i = tl.count() - 1; i > -1; i--) {
            updateTable(dr, db, step, steps, tl.at(i));
        }
        db.commit();
    }
    dr[":f_id"] = lastid;
    dr.exec("select s.f_id, s.f_table, s.f_recid, s.f_op "
            "from s_syncronize s "
            "inner join s_syncronize_in si on si.f_table=s.f_table "
            "where s.f_id>:f_id");
    while (dr.nextRow()) {
        emit progress(QString("%1 started").arg(dr.getString("f_table")));
        hc = true;
        switch (dr.getInt("f_op")) {
        case 1:
            dr2[":f_id"] = dr.getInt("f_recid");
            dr2.exec("select * from " + dr.getString("f_table") + " where f_id=:f_id");
            if (dr2.nextRow()) {
                db.setBindValues(dr2.getBindValues());
                db.insert(dr.getString("f_table"), false);
            }
            break;
        case 2:
            dr2[":f_id"] = dr.getString("f_recid");
            dr2.exec("select * from " + dr.getString("f_table") + " where f_id=:f_id");
            if (dr2.nextRow()) {
                db.setBindValues(dr2.getBindValues());
                db.update(dr.getString("f_table"), where_id(dr.getString("f_recid")));
            }
            break;
        case 3:
            db[":f_id"] = dr.getString("f_recid");
            db.exec("delete from " + dr.getString("f_table") + " where f_id=:f_id");
            break;
        }
        lastid = dr.getInt("f_id");
        emit progress(QString("Step %1 of %2. Items %3 of %4").arg(step).arg(steps).arg(dr.pos()).arg(dr.rowCount()));
    }
    db[":f_host"] = db.fDb.hostName();
    db[":f_db"] = db.fDb.databaseName();
    db[":f_syncin"] = lastid;
    db.exec("update s_db set f_syncin=:f_syncin, f_synctime=current_timestamp where f_host=:f_host and f_db=:f_db");

    db.exec("delete from a_store_temp");
    dr[":date"] = QDate::currentDate();
    dr[":store"] = __c5config.defaultStore();
    dr[":state"] = DOC_STATE_SAVED;
    dr.exec("select g.f_id as f_code,ss.f_name as f_storage,gg.f_name as f_group,g.f_name as f_goods,g.f_scancode,sum(s.f_qty*s.f_type) as f_qty, "
            "u.f_name as f_unit,g.f_lastinputprice,sum(g.f_lastinputprice*s.f_type*s.f_qty) as f_total,g.f_saleprice,sum(s.f_qty*s.f_type)*g.f_saleprice as f_totalsale,"
            "g.f_saleprice2,sum(s.f_qty*s.f_type)*g.f_saleprice2 as f_totalsale2 "
            "from a_store_draft s "
            "inner join c_goods g on g.f_id=s.f_goods "
            "inner join c_storages ss on ss.f_id=s.f_store "
            "inner join c_groups gg on gg.f_id=g.f_group "
            "inner join c_units u on u.f_id=g.f_unit "
            "inner join a_header h on h.f_id=s.f_document  "
            "where  h.f_date<=:date  and s.f_store=:store and h.f_state=:state "
            "group by g.f_id,ss.f_name,gg.f_name,g.f_name,u.f_name,g.f_lastinputprice,g.f_saleprice,g.f_saleprice2 "
            "having sum(s.f_qty*s.f_type) > 0");
    while (dr.nextRow()) {
        db[":f_store"] = __c5config.defaultStore();
        db[":f_goods"] = dr.getInt("f_code");
        db[":f_qty"] = dr.getDouble("f_qty");
        db.insert("a_store_temp", false);
        if (dr.pos() % 20 == 0) {
            emit progress(QString("Step %1 of %2. %3 %4/%5").arg(step).arg(steps).arg("Store").arg(dr.pos()).arg(dr.rowCount()));
        }
    }
    step++;
    emit haveChanges(hc);
    emit finished();
}

bool C5Replication::uploadDataToServer(const QStringList &src, const QStringList &dst)
{
    C5Database dr(dst);
    if (!dr.open()) {
        return false;
    }
    C5Database db(src);
    QStringList idlist;
    db.exec("select f_id from o_header");
    while (db.nextRow()) {
        idlist.append(db.getString("f_id"));
    }
    foreach (const QString &s, idlist) {
        db.startTransaction();
        dr.startTransaction();

        db[":f_id"] = s;
        db.exec("select * from o_header where f_id=:f_id");
        if (!db.nextRow()) {
            return ret(dr, db);
        }
        dr.setBindValues(db.getBindValues());
        if (!dr.insert("o_header", false)) {
            if (!fIgnoreErrors) {
                return ret(dr, db);
            }
        }

        QSet<QString> storeRec;
        db[":f_header"] = s;
        db.exec("select * from o_goods where f_header=:f_header");
        while (db.nextRow()) {
            dr.setBindValues(db.getBindValues());
            storeRec.insert(db.getString("f_storerec"));
            if (!dr.insert("o_goods", false)) {
                if (!fIgnoreErrors) {
                    return ret(dr, db);
                }
            }
        }

        db[":f_header"] = s;
        db.exec("select * from o_body where f_header=:f_header");
        while (db.nextRow()) {
            dr.setBindValues(db.getBindValues());
            if (!dr.insert("o_body", false)) {
                if (!fIgnoreErrors) {
                    return ret(dr, db);
                }
            }
        }

        db[":f_order"] = s;
        db.exec("select * from o_tax_log where f_order=:f_order");
        while (db.nextRow()) {
            dr.setBindValues(db.getBindValues());
            dr.insert("o_tax_log", false);
        }

        db[":f_id"] = s;
        db.exec("select * from o_tax where f_id=:f_id");
        if (db.nextRow()) {
            dr.setBindValues(db.getBindValues());
            dr.insert("o_tax", false);
        }

        QSet<QString> storeHeader;
        foreach (const QString &s1, storeRec) {
            db[":f_id"] = s1;
            db.exec("select f_document from a_store_draft where f_id=:f_id");
            if (db.nextRow()) {
                storeHeader.insert(db.getString("f_document"));
            }
        }
        foreach (const QString &s1, storeHeader) {
            db[":f_id"] = s1;
            db.exec("select * from a_header where f_id=:f_id");
            if (db.nextRow()) {
                dr.setBindValues(db.getBindValues());
                if (!dr.insert("a_header", false)) {
                    if (!fIgnoreErrors) {
                        return ret(dr, db);
                    }
                }
            }
            db[":f_id"] = s1;
            db.exec("select * from a_header_store where f_id=:f_id");
            if (db.nextRow()) {
                dr.setBindValues(db.getBindValues());
                if (!dr.insert("a_header_store", false)) {
                    if (!fIgnoreErrors) {
                        return ret(dr, db);
                    }
                }
            }
            db[":f_document"] = s1;
            db.exec("select * from a_store_draft where f_document=:f_document");
            while (db.nextRow()) {
                dr.setBindValues(db.getBindValues());
                if (!dr.insert("a_store_draft", false)) {
                    if (!fIgnoreErrors) {
                        return ret(dr, db);
                    }
                }
            }
        }

        QSet<QString> cashHeader;
        db[":f_oheader"] = s;
        db.exec("select * from a_header_cash where f_oheader=:f_oheader");
        while (db.nextRow()) {
            cashHeader.insert(db.getString("f_id"));
            dr.setBindValues(db.getBindValues());
            if (!dr.insert("a_header_cash", false)) {
                if (!fIgnoreErrors) {
                    return ret(dr, db);
                }
            }
        }
        foreach (const QString &s1, cashHeader) {
            db[":f_id"] = s1;
            db.exec("select * from a_header where f_id=:f_id");
            if (db.nextRow()) {
                dr.setBindValues(db.getBindValues());
                if (!dr.insert("a_header", false)) {
                    if (!fIgnoreErrors) {
                        return ret(dr, db);
                    }
                }
            }
            db[":f_header"] = s1;
            db.exec("select * from e_cash where f_header=:f_header");
            while (db.nextRow()) {
                dr.setBindValues(db.getBindValues());
                if (!dr.insert("e_cash", false)) {
                    if (!fIgnoreErrors) {
                        return ret(dr, db);
                    }
                }
            }
        }

        QString err;
        C5StoreDraftWriter dw(dr);
        foreach (const QString &s1, storeHeader) {
            if (dw.writeOutput(s1, err)) {
                dr[":f_id"] = s1;
                dr[":f_state"] = DOC_STATE_SAVED;
                dr.exec("update a_header set f_state=:f_state where f_id=:f_id");
            } else {
                dw.haveRelations(s1, err, true);
            }
        }

        db[":f_header"] = s;
        db.exec("delete from o_goods where f_header=:f_header");
        db[":f_header"] = s;
        db.exec("delete from o_body where f_header=:f_header");
        db[":f_id"] = s;
        db.exec("delete from o_header where f_id=:f_id");
        db[":f_order"] = s;
        db.exec("delete from o_tax_log where f_order=:f_order");
        db[":f_id"] = s;
        db.exec("delete from o_tax where f_id=:f_id");
        foreach (const QString &s1, storeHeader) {
            db[":f_document"] = s1;
            db.exec("delete from a_store where f_document=:f_document");
            db[":f_document"] = s1;
            db.exec("delete from a_store_draft where f_document=:f_document");
            db[":f_id"] = s1;
            db.exec("delete from a_header_store where f_id=:f_id");
            db[":f_id"] = s1;
            db.exec("delete from a_header where f_id=:f_id");
        }
        foreach (const QString &s1, cashHeader) {
            db[":f_header"] = s1;
            db.exec("delete from e_cash where f_header=:f_header");
            db[":f_id"] = s1;
            db.exec("delete from a_header_cash where f_id=:f_id");
            db[":f_id"] = s1;
            db.exec("delete from a_header where f_id=:f_id");
        }
        db.commit();
        dr.commit();
    }
    emit finished();
    return true;
}

bool C5Replication::uploadToServer()
{
    return uploadDataToServer(__c5config.dbParams(), __c5config.replicaDbParams());
}

void C5Replication::updateTable(C5Database &dr, C5Database &db, int &step, int steps, const QString &tableName)
{
    emit progress(QString("%1 updatetable").arg(tableName));
    dr.exec("select * from " + tableName);
    while (dr.nextRow()) {
        if (dr.pos() % 10 == 0) {
            emit progress(QString("Update table. Step %1 of %2. %3 %4/%5").arg(step).arg(steps).arg(tableName).arg(dr.pos()).arg(dr.rowCount()));
        }
        db.setBindValues(dr.getBindValues());
        if (!db.insert(tableName, false)) {
            emit progress(db.fLastError);
        }
    }
    step++;
}
