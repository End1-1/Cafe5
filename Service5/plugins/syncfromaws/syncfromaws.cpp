#include "syncfromaws.h"
#include "worker.h"
#include <QDebug>
#include <QTextCodec>

WaiterClientHandler *waiterClientHandler = nullptr;

bool startPlugin(const QString &configFileName)
{
    qDebug() << "start plugin syncfromaws with valshin" ;
    waiterClientHandler = new WaiterClientHandler();
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf8") );
    return true;
}

QString pluginUuid()
{
    return "6689a151-fa8a-11ee-ae46-7c10c9bcac82";
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
