#include "jzsync.h"
#include "jzsyncmanager.h"
#include "database.h"
#include "databaseconnectionmanager.h"
#include <QDebug>

bool startPlugin(const QString &configFileName)
{
    qDebug() << "start plugin with" << configFileName;
    Database db("QMYSQL");
    if (!db.open(configFileName)) {
        return false;
    }
    db[":fplugin"] = "jzsync";
    db.exec("select * from plugins_options where fplugin=:fplugin");
    QHash<QString, QString> opt;
    while (db.next()) {
        opt[db.string("fkey")] = db.string("fvalue");
    }
    opt["server_db_config"] = configFileName;
    qDebug() << opt;
    auto *syncmanager = new JzSyncManager(opt);
    syncmanager->start();
    return true;
}
