#include "waiterclientp.h"
#include "database.h"
#include "waiterclienthandler.h"
#include <QDebug>

WaiterClientHandler *waiterClientHandler = nullptr;

bool startPlugin(const QString &configFileName)
{
    qDebug() << "start plugin waiterclientp with" << configFileName;
    Database db("QMYSQL");
    if (!db.open(configFileName)) {
        return false;
    }
    db[":fplugin"] = "waiterclientp";
    db.exec("select * from plugins_options where fplugin=:fplugin");
    QHash<QString, QString> opt;
    while (db.next()) {
        opt[db.string("fkey")] = db.string("fvalue");
    }
    opt["server_db_config"] = configFileName;
    qDebug() << opt;
    waiterClientHandler = new WaiterClientHandler(opt);
    return true;
}

QString pluginUuid()
{
    return "8b90e61a-1385-4fb4-85ce-f23849045e69";
}

int handler(RawMessage *rm, const QByteArray &in)
{
    qDebug() << "Plugin handler started: " << rm->socket()->fUuid;
    waiterClientHandler->run(rm, in);
    return 0;
}

int removeSocket(SslSocket *s)
{
    waiterClientHandler->removeSocket(s);
    return 0;
}
