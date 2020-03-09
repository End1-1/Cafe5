#include "c5pointsync.h"
#include "c5database.h"
#include "c5filelogwriter.h"
#include <QTimer>

#define sop_insert 1
#define sop_update 2
#define sop_delete 3

C5PointSync::C5PointSync(const QStringList &mainDbParams, const QStringList &dbParams, QObject *parent) :
    C5ThreadObject(parent),
    fMainDbParams(mainDbParams),
    fDbParams(dbParams)
{

}

void C5PointSync::run()
{
    QTimer *t = new QTimer(this);
    t->setInterval(10000);
    connect(t, SIGNAL(timeout()), this, SLOT(timeout()));
    t->start();
}

void C5PointSync::doSync()
{
    QString logfile("sync_" + fDbParams.at(1) + ".log");
    c5logf(QString("Starting synchronization."), logfile);
    C5Database dm(fMainDbParams.at(2), fMainDbParams.at(3), fMainDbParams.at(4), fMainDbParams.at(5));
    if (!dm.open()) {
        c5logf(QString("Master database inaccessible."), logfile);
        return;
    }
    c5logf(QString("Connected to master database."), logfile);
    C5Database ds(fDbParams.at(2), fDbParams.at(3), fDbParams.at(4), fDbParams.at(5));
    if (!ds.open()) {
        c5logf(QString("Slave database inaccessible."), logfile);
        return;
    }
    c5logf(QString("Connected to slave database."), logfile);
    QStringList tablesIn;
    QStringList tablesOut;
    c5logf(QString("Select input tables."), logfile);
    dm.exec("select f_table from s_syncronize_in");
    while (dm.nextRow()) {
        tablesIn.append(dm.getString(0));
    }
    c5logf(QString("Select output tables."), logfile);
    dm.exec("show tables");
    while (dm.nextRow()) {
        if (tablesIn.contains(dm.getString(0))) {
            continue;
        }
        tablesOut.append(dm.getString(0));
    }
    c5logd("Start output tables synchronization.", logfile);
    dm[":f_id"] = fDbParams.at(0);
    dm.exec("select f_syncout from s_db where f_id=:f_id");
    if (!dm.nextRow()) {
        c5logd("Invalid database id. Could not fetch f_syncout field.", logfile);
        return;
    }
    int maxId = dm.getInt(0);
    QList<int> ids;
    dm[":f_id"] = maxId;
    dm.exec("select f_id, f_table from s_syncronize where f_id>:f_id");
    while (dm.nextRow()) {
        if (!tablesOut.contains(dm.getString(1))) {
            continue;
        }
        ids.append(dm.getInt("f_id"));
    }
    foreach (int id, ids) {
        dm[":f_id"] = id;
        dm.exec("select * from s_syncronize where f_id=:f_id");
        dm.nextRow();
        QString table = dm.getString("f_table");
        int op = dm.getInt("f_op");
        QString recid = dm.getString("f_recid");
        dm[":f_id"] = recid;
        dm.exec(QString("select * from %1 where f_id=:f_id").arg(table));
        bool hasrecord = dm.nextRow();
        switch (op) {
        case sop_insert:
            if (!hasrecord) {
                c5logf(QString("Insert skiped. Missing record. Table %1, ID %2").arg(table).arg(recid), logfile);
                continue;
            }
            ds.fBindValues = dm.getBindValues();
            if (!ds.insert(table, false)) {
                c5logf(QString("Insert failed. Table %1, ID %2. %3").arg(table).arg(recid).arg(ds.fLastError), logfile);
                continue;
            }
            break;
        case sop_update:
            if (!hasrecord) {
                c5logf(QString("Update skiped. Missing record. Table %1, ID %2").arg(table).arg(recid), logfile);
                continue;
            }
            ds.fBindValues = dm.getBindValues();
            if (!ds.update(table, where_id(recid))) {
                c5logf(QString("Update failed. Table %1, ID %2. %3").arg(table).arg(recid).arg(ds.fLastError), logfile);
                continue;
            }
            break;
        case sop_delete:
            ds[":f_id"] = recid;
            ds.exec(QString("delete from %1 where f_id=:f_id").arg(table));
            break;
        }
        maxId = maxId > id ? maxId : id;
    }
    dm[":f_syncout"] = maxId;
    dm[":f_id"] = fDbParams.at(0);
    dm.exec("update s_db set f_syncout=:f_syncout where f_id=:f_id");

    c5logd("Start input tables synchronization.", logfile);
    dm[":f_id"] = fDbParams.at(0);
    dm.exec("select f_syncin from s_db where f_id=:f_id");
    if (!dm.nextRow()) {
        c5logd("Invalid database id. Could not fetch f_syncout field.", logfile);
        return;
    }
    maxId = dm.getInt(0);
    ids.clear();
    ds[":f_id"] = maxId;
    ds.exec("select f_id, f_table from s_syncronize where f_id>:f_id");
    while (ds.nextRow()) {
        if (!tablesIn.contains(ds.getString(1))) {
            continue;
        }
        ids.append(ds.getInt("f_id"));
    }
    foreach (int id, ids) {
        ds[":f_id"] = id;
        ds.exec("select * from s_syncronize where f_id=:f_id");
        ds.nextRow();
        QString table = ds.getString("f_table");
        int op = ds.getInt("f_op");
        QString recid = ds.getString("f_recid");
        ds[":f_id"] = recid;
        ds.exec(QString("select * from %1 where f_id=:f_id").arg(table));
        bool hasrecord = ds.nextRow();
        switch (op) {
        case sop_insert:
            if (!hasrecord) {
                c5logf(QString("Insert skiped. Missing record. Table %1, ID %2").arg(table).arg(recid), logfile);
                continue;
            }
            dm.fBindValues = ds.getBindValues();
            if (!dm.insert(table, false)) {
                c5logf(QString("Insert failed. Table %1, ID %2. %3").arg(table).arg(recid).arg(dm.fLastError), logfile);
                continue;
            }
            break;
        case sop_update:
            if (!hasrecord) {
                c5logf(QString("Update skiped. Missing record. Table %1, ID %2").arg(table).arg(recid), logfile);
                continue;
            }
            dm.fBindValues = ds.getBindValues();
            if (!dm.update(table, where_id(recid))) {
                c5logf(QString("Update failed. Table %1, ID %2. %3").arg(table).arg(recid).arg(dm.fLastError), logfile);
                continue;
            }
            break;
        case sop_delete:
            dm[":f_id"] = recid;
            dm.exec(QString("delete from %1 where f_id=:f_id").arg(table));
            break;
        }
        maxId = maxId > id ? maxId : id;
    }
    dm[":f_syncin"] = maxId;
    dm[":f_synctime"] = QDateTime::currentDateTime();
    dm[":f_id"] = fDbParams.at(0);
    dm.exec("update s_db set f_syncin=:f_syncin where f_id=:f_id");
    c5logd(QString("Synchronization finished. Last ID: %1").arg(maxId), logfile);
}

void C5PointSync::timeout()
{
    QTimer *t = static_cast<QTimer*>(sender());
    t->stop();
    doSync();
    t->start();
}
