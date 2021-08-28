#include "c5replication.h"
#include "c5config.h"
#include "c5utils.h"
#include "c5shoporder.h"
#include "c5storedoc.h"
#include <QThread>
#include <QUuid>

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
    emit progress("Connected");
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
        db.exec("select f_id from a_header where f_userid='SL'");
        if (db.nextRow()) {
            QString sl = db.getString(0);
            C5StoreDoc::removeDoc(db.dbParams(), sl, false);
        }
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

    emit progress("Clear temp storage");
    db[":doc_type"] = DOC_TYPE_SALE_INPUT;
    db.exec("delete from a_store_draft where f_document in (select f_id from a_header where f_type=:doc_type)");
    db[":doc_type"] = DOC_TYPE_SALE_INPUT;
    db.exec("delete from a_store where f_document in (select f_id from a_header where f_type=:doc_type)");
    db.exec("delete from a_store_current");
    db[":doc_type"] = DOC_TYPE_SALE_INPUT;
    db.exec("delete from a_header where f_type=:doc_type ");
    dr[":store"] = __c5config.defaultStore();
    dr.exec("select s.f_base, s.f_goods, s.f_price, sum(s.f_qty*s.f_type) as f_qty, sum(s.f_total) as f_total "
            "from a_store s "
            "where s.f_store=:store "
            "group by 1, 2 "
            "having sum(s.f_qty*s.f_type)>0 ");

    C5StoreDraftWriter dw(db);
    int rownum = 1;
    QString docId;
    dw.writeAHeader(docId, "SL", DOC_STATE_SAVED, DOC_TYPE_SALE_INPUT, 1, QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0, 0, "", 0, 0);
    emit progress(QString("Step %1 of %2. Write header").arg(step).arg(steps));
    QString logsql = "insert into a_store_draft (f_id, f_document, f_store, f_type, f_goods, f_qty, f_price, f_total, f_base, f_row, f_comment) values ";
    bool f = true;
    while (dr.nextRow()) {
        if (f) {
            f = false;
        } else {
            logsql.append(",");
        }
        QString u = QUuid::createUuid().toString().replace("{", "").replace("}", "");
        logsql.append(QString("('%1', '%2', %3, %4, %5, %6, %7, %8, '', %9, '')")
                      .arg(u)
                      .arg(docId)
                      .arg(__c5config.defaultStore())
                      .arg(DOC_TYPE_STORE_INPUT)
                      .arg(dr.getInt("f_goods"))
                      .arg(dr.getDouble("f_qty"))
                      .arg(dr.getDouble("f_price"))
                      .arg(dr.getDouble("f_total"))
                      .arg(rownum++));
//        QString id;
//        dw.writeAStoreDraft(id, docId, __c5config.defaultStore(), DOC_TYPE_STORE_INPUT, dr.getInt("f_goods"),
//                            dr.getDouble("f_qty"), dr.getDouble("f_price"),
//                            dr.getDouble("f_total"), DOC_REASON_INPUT, id, rownum++, "");
//        db.setBindValues(dr.getBindValues());
        emit progress(QString("Step %1 of %2. %3 %4/%5").arg(step).arg(steps).arg("Store").arg(dr.pos()).arg(dr.rowCount()));
    }
    if(!db.exec(logsql)) {
        emit progress(db.fLastError);
        emit finished();
        return;
    }
    emit progress(QString("Step %1 of %2. Store document: write input").arg(step).arg(steps));
    QString err;
    if (!dw.writeInput(docId, err)) {
        emit progress(err);
    }
    step++;
    emit haveChanges(hc);
    emit finished();
}

bool C5Replication::uploadDataToServer(const QStringList &src, const QStringList &dst)
{
    fIgnoreErrors = false;
    C5Database db_dst(dst);
    if (!db_dst.open()) {
        return false;
    }
    C5Database db_src(src);
    if (!db_src.open()) {
        return false;
    }
    QStringList idlist;
    db_src.exec("select f_id from o_header");
    while (db_src.nextRow()) {
        idlist.append(db_src.getString("f_id"));
    }
    foreach (const QString &s, idlist) {
        db_src.startTransaction();
        db_dst.startTransaction();

        db_src[":f_id"] = s;
        db_src.exec("select * from o_header where f_id=:f_id ");
        if (!db_src.nextRow()) {
            qDebug() << db_src.fDb.hostName() << db_src.fDb.databaseName();
            return ret(db_dst, db_src);
        }
        db_dst.setBindValues(db_src.getBindValues());
        if (!db_dst.insert("o_header", false)) {
            if (!fIgnoreErrors) {
                return ret(db_dst, db_src);
            }
        }

        db_src[":f_id"] = s;
        db_src.exec("select * from o_header_flags where f_id=:f_id");
        if (!db_src.nextRow()) {
            qDebug() << db_src.fDb.hostName() << db_src.fDb.databaseName();
            return ret(db_dst, db_src);
        }
        db_dst.setBindValues(db_src.getBindValues());
        if (!db_dst.insert("o_header_flags", false)) {
            if (!fIgnoreErrors) {
                return ret(db_dst, db_src);
            }
        }

        QSet<QString> storeRec;
        db_src[":f_header"] = s;
        db_src.exec("select * from o_goods where f_header=:f_header");
        while (db_src.nextRow()) {
            db_dst.setBindValues(db_src.getBindValues());
            storeRec.insert(db_src.getString("f_storerec"));
            if (!db_dst.insert("o_goods", false)) {
                if (!fIgnoreErrors) {
                    return ret(db_dst, db_src);
                }
            }
        }

        db_src[":f_header"] = s;
        db_src.exec("select * from o_body where f_header=:f_header");
        while (db_src.nextRow()) {
            db_dst.setBindValues(db_src.getBindValues());
            if (!db_dst.insert("o_body", false)) {
                if (!fIgnoreErrors) {
                    return ret(db_dst, db_src);
                }
            }
        }

        db_src[":f_order"] = s;
        db_src.exec("select * from o_tax_log where f_order=:f_order");
        while (db_src.nextRow()) {
            db_dst.setBindValues(db_src.getBindValues());
            db_dst.insert("o_tax_log", false);
        }

        db_src[":f_id"] = s;
        db_src.exec("select * from o_tax where f_id=:f_id");
        if (db_src.nextRow()) {
            db_dst.setBindValues(db_src.getBindValues());
            db_dst.insert("o_tax", false);
        }

        QSet<QString> storeHeader;
        foreach (const QString &s1, storeRec) {
            db_src[":f_id"] = s1;
            db_src.exec("select f_document from a_store_draft where f_id=:f_id");
            if (db_src.nextRow()) {
                storeHeader.insert(db_src.getString("f_document"));
            }
        }
        foreach (const QString &s1, storeHeader) {
            db_src[":f_id"] = s1;
            db_src.exec("select * from a_header where f_id=:f_id");
            if (db_src.nextRow()) {
                db_dst.setBindValues(db_src.getBindValues());
                if (!db_dst.insert("a_header", false)) {
                    if (!fIgnoreErrors) {
                        return ret(db_dst, db_src);
                    }
                }
            }
            db_src[":f_id"] = s1;
            db_src.exec("select * from a_header_store where f_id=:f_id");
            if (db_src.nextRow()) {
                db_dst.setBindValues(db_src.getBindValues());
                if (!db_dst.insert("a_header_store", false)) {
                    if (!fIgnoreErrors) {
                        return ret(db_dst, db_src);
                    }
                }
            }
            db_src[":f_document"] = s1;
            db_src.exec("select * from a_store_draft where f_document=:f_document");
            while (db_src.nextRow()) {
                db_dst.setBindValues(db_src.getBindValues());
                if (!db_dst.insert("a_store_draft", false)) {
                    if (!fIgnoreErrors) {
                        return ret(db_dst, db_src);
                    }
                }
            }
        }

        QSet<QString> cashHeader;
        db_src[":f_oheader"] = s;
        db_src.exec("select * from a_header_cash where f_oheader=:f_oheader");
        while (db_src.nextRow()) {
            cashHeader.insert(db_src.getString("f_id"));
            db_dst.setBindValues(db_src.getBindValues());
            if (!db_dst.insert("a_header_cash", false)) {
                if (!fIgnoreErrors) {
                    return ret(db_dst, db_src);
                }
            }
        }
        foreach (const QString &s1, cashHeader) {
            db_src[":f_id"] = s1;
            db_src.exec("select * from a_header where f_id=:f_id");
            if (db_src.nextRow()) {
                db_dst.setBindValues(db_src.getBindValues());
                if (!db_dst.insert("a_header", false)) {
                    if (!fIgnoreErrors) {
                        return ret(db_dst, db_src);
                    }
                }
            }
            db_src[":f_header"] = s1;
            db_src.exec("select * from e_cash where f_header=:f_header");
            while (db_src.nextRow()) {
                db_dst.setBindValues(db_src.getBindValues());
                if (!db_dst.insert("e_cash", false)) {
                    if (!fIgnoreErrors) {
                        return ret(db_dst, db_src);
                    }
                }
            }
        }

        QString err;
        C5StoreDraftWriter dw(db_dst);
        foreach (const QString &s1, storeHeader) {
            if (dw.writeOutput(s1, err)) {
                db_dst[":f_id"] = s1;
                db_dst[":f_state"] = DOC_STATE_SAVED;
                db_dst.exec("update a_header set f_state=:f_state where f_id=:f_id");
            } else {
                dw.haveRelations(s1, err, true);
            }
        }

        db_src[":f_header"] = s;
        db_src.exec("delete from o_goods where f_header=:f_header");
        db_src[":f_header"] = s;
        db_src.exec("delete from o_body where f_header=:f_header");
        db_src[":f_id"] = s;
        db_src.exec("delete from o_header where f_id=:f_id");
        db_src[":f_order"] = s;
        db_src.exec("delete from o_tax_log where f_order=:f_order");
        db_src[":f_id"] = s;
        db_src.exec("delete from o_tax where f_id=:f_id");
        foreach (const QString &s1, storeHeader) {
            db_src[":f_document"] = s1;
            db_src.exec("delete from a_store where f_document=:f_document");
            db_src[":f_document"] = s1;
            db_src.exec("delete from a_store_draft where f_document=:f_document");
            db_src[":f_id"] = s1;
            db_src.exec("delete from a_header_store where f_id=:f_id");
            db_src[":f_id"] = s1;
            db_src.exec("delete from a_header where f_id=:f_id");
        }
        foreach (const QString &s1, cashHeader) {
            db_src[":f_header"] = s1;
            db_src.exec("delete from e_cash where f_header=:f_header");
            db_src[":f_id"] = s1;
            db_src.exec("delete from a_header_cash where f_id=:f_id");
            db_src[":f_id"] = s1;
            db_src.exec("delete from a_header where f_id=:f_id");
        }
        db_src.commit();
        db_dst.commit();
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
        if (dr.pos() % 5 == 0) {
            emit progress(QString("Update table. Step %1 of %2. %3 %4/%5").arg(step).arg(steps).arg(tableName).arg(dr.pos()).arg(dr.rowCount()));
        }
        db.setBindValues(dr.getBindValues());
        if (!db.insert(tableName, false)) {
            emit progress(db.fLastError);
        }
    }
    step++;
}
