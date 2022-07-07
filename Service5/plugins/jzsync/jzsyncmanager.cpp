#include "jzsyncmanager.h"
#include "database.h"
#include "logwriter.h"
#include <QTimer>
#include <QDebug>

JzSyncManager::JzSyncManager(const QHash<QString, QString> &opt) :
    QObject()
{
    fOptions = opt;
}

void JzSyncManager::start()
{
    fTimer = new QTimer(this);
    connect(fTimer, &QTimer::timeout, this, &JzSyncManager::timeout);
    resumeTimer();
}

void JzSyncManager::resumeTimer()
{
#ifdef QT_DEBUG
    fTimer->start(500);
#else
    fTimer->start(5000);
#endif
}

void JzSyncManager::timeout()
{
    fTimer->stop();
    Database db("QIBASE");
    if (!db.open(fOptions["db_remote_host"], fOptions["db_remote_path"], fOptions["db_remote_user"], fOptions["db_remote_pass"])) {
        resumeTimer();
        return;
    }
    Database db2("QIBASE");
    if (!db2.open(fOptions["db_local_host"], fOptions["db_local_path"], fOptions["db_local_user"], fOptions["db_local_pass"])) {
        resumeTimer();
        return;
    }
    db[":fid"] = fOptions["last_id"].toInt();
    db.exec("select * from sys_sync where fid>:fid");
    QStringList ops, rec, tables;
    while (db.next()) {
        fOptions["last_id"] = QString::number(db.integer("fid"));
        ops.append(db.string("fop"));
        rec.append(db.string("frec"));
        tables.append(db.string("ftable"));
    }
    for (int i = 0; i < ops.count(); i++) {
        switch (ops.at(i).toInt()) {
        case 1:
            db[":id"] = rec.at(i);
            db.exec(QString("select * from %1 where id=:id").arg(tables.at(i)));
            if (db.next()) {
                QMap<QString, QVariant> v = db.getBindValues();
                db2.setBindValues(v);
                db2.insert(tables.at(i));
            } else {
                LogWriter::write(LogWriterLevel::errors, "", QString("Record missing: %1 - %2").arg(tables.at(i), rec.at(i)));
            }
            break;
        case 2:
            db[":id"] = rec.at(i);
            db.exec(QString("select * from %1 where id=:id").arg(tables.at(i)));
            if (db.next()) {
                QMap<QString, QVariant> v = db.getBindValues();
                qDebug() << v;
                db2.setBindValues(v);
                db2.update(tables.at(i), "id", v[":id"]);
            } else {
                LogWriter::write(LogWriterLevel::errors, "", QString("Record missing: %1 - %2").arg(tables.at(i), rec.at(i)));
            }
            break;
        case 3:
            db[":id"] = rec.at(i);
            db.deleteFromTable(tables.at(i), "id", rec.at(i));
            break;
        }
    }
    Database db3("QMYSQL");
    if (!db3.open(fOptions["server_db_config"])) {
        resumeTimer();
        return;
    }
    db3[":fplugin"] = "jzsync";
    db3[":fkey"] = "last_id";
    db3[":fvalue"] = fOptions["last_id"];
    db3.exec("update plugins_options set fvalue=:fvalue where fplugin=:fplugin and fkey=:fkey");
    resumeTimer();
}
