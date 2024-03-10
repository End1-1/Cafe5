#include "shopclientp.h"
#include "database.h"
#include "shopclienthandler.h"
#include <QDebug>
#include <QTextCodec>

ShopClientHandler *shopClientHandler = nullptr;

bool startPlugin(const QString &configFileName)
{
    qDebug() << "start plugin shopclientp with" << configFileName;
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8") );
    Database db("QMARIADB");
    if (!db.open(configFileName)) {
        return false;
    }
    db[":fplugin"] = "shopclientp";
    db.exec("select * from plugins_options where fplugin=:fplugin");
    QHash<QString, QString> opt;
    while (db.next()) {
        opt[db.string("fkey")] = db.string("fvalue");
    }
    opt["server_db_config"] = configFileName;
    qDebug() << opt;
    shopClientHandler = new ShopClientHandler(opt);
    return true;
}

QString pluginUuid()
{
    return "0afc8abf-95d2-48d9-bc34-358714715d60";
}

int handler(RawMessage *rm, const QByteArray &in)
{
    qDebug() << "Plugin handler started: " << rm->socket()->fUuid;
    shopClientHandler->run(rm, in);
    return 0;
}

int removeSocket(SslSocket *s)
{
    shopClientHandler->removeSocket(s);
    return 0;
}
