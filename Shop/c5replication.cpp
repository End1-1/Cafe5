#include "c5replication.h"
#include "c5config.h"
#include "c5utils.h"
#include "c5shoporder.h"
#include <QThread>

C5Replication::C5Replication(QObject *parent) :
    QObject(parent)
{

}

void C5Replication::start()
{
    QThread *t = new QThread();
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
    connect(t, SIGNAL(finished()), this, SLOT(deleteLater()));
    connect(t, SIGNAL(started()), this, SLOT(process()));
    connect(this, SIGNAL(finished()), t, SLOT(quit()));
    setParent(0);
    moveToThread(t);
    t->start();
}

bool C5Replication::ret(C5Database &db1, C5Database &db2)
{
    db1.rollback();
    db2.rollback();
    emit finished();
    return true;
}

bool C5Replication::process()
{
    C5Database dr(__c5config.replicaDbParams());
    C5Database db(__c5config.dbParams());
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
            return ret(dr, db);
        }

        QSet<QString> storeRec;
        db[":f_header"] = s;
        db.exec("select * from o_goods where f_header=:f_header");
        while (db.nextRow()) {
            dr.setBindValues(db.getBindValues());
            storeRec.insert(db.getString("f_storerec"));
            if (!dr.insert("o_goods", false)) {
                return ret(dr, db);
            }
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
                    return ret(dr, db);
                }
            }
            db[":f_id"] = s1;
            db.exec("select * from a_header_store where f_id=:f_id");
            if (db.nextRow()) {
                dr.setBindValues(db.getBindValues());
                if (!dr.insert("a_header_store", false)) {
                    return ret(dr, db);
                }
            }
            db[":f_document"] = s1;
            db.exec("select * from a_store_draft where f_document=:f_document");
            while (db.nextRow()) {
                dr.setBindValues(db.getBindValues());
                if (!dr.insert("a_store_draft", false)) {
                    return ret(dr, db);
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
                return ret(dr, db);
            }
        }
        foreach (const QString &s1, cashHeader) {
            db[":f_id"] = s1;
            db.exec("select * from a_header where f_id=:f_id");
            if (db.nextRow()) {
                dr.setBindValues(db.getBindValues());
                if (!dr.insert("a_header", false)) {
                    return ret(dr, db);
                }
            }
            db[":f_header"] = s1;
            db.exec("select * from e_cash where f_header=:f_header");
            while (db.nextRow()) {
                dr.setBindValues(db.getBindValues());
                if (!dr.insert("e_cash", false)) {
                    return ret(dr, db);
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
        db[":f_id"] = s;
        db.exec("delete from o_header where f_id=:f_id");
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
