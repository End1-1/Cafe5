#include "shopclienthandler.h"
#include <QThread>
#include <QTimer>

ShopClientHandler::ShopClientHandler(QHash<QString, QString> conn, QObject *parent) :
    QObject(parent),
    fDbConnectionSettings(conn),
    fTaskCounter(0)
{
    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ShopClientHandler::tasks);
    timer->start(1000);
}

ShopClientHandler::~ShopClientHandler()
{
    qDebug() << "~ShopClientHandler()";
}

void ShopClientHandler::run(RawMessage *rm, const QByteArray &in)
{
    if (!fConnections.contains(rm->socket()->fUuid)) {
        qDebug() << "Created new plugin connection" << rm->socket()->fUuid;
        auto *w = new ShopConnection(fDbConnectionSettings, rm->socket());
        connect(this, &ShopClientHandler::handleData, w, &ShopConnection::handleData);
        connect(this, &ShopClientHandler::removeSocketHolder, w, &ShopConnection::removeSocket);
        connect(w, &ShopConnection::broadcastForClients, this, &ShopClientHandler::broadcastForClients);
        connect(w, &ShopConnection::removeMeFromConnectionList, this, &ShopClientHandler::removeMeFromConnectionList);
        connect(this, &ShopClientHandler::sendToAllClients, w, &ShopConnection::writeToSocket);
        QThread *t = new QThread();
        connect(w, &ShopConnection::destroyed, t, &QThread::deleteLater);
        w->moveToThread(t);
        t->start();
        fConnections[rm->socket()->fUuid] = w;
    }
    qDebug() << "emit handledata for" << rm->socket()->fUuid;
    emit handleData(rm, in);
}

void ShopClientHandler::removeSocket(SslSocket *s)
{
    emit removeSocketHolder(s);
}

void ShopClientHandler::tasks()
{
    fTaskCounter++;
    if (fTaskCounter %10) {

    }
}

void ShopClientHandler::broadcastForClients(QByteArray data)
{
    qDebug() << "Broadcast for clients";
    emit sendToAllClients(data);
}

void ShopClientHandler::removeMeFromConnectionList(QString uuid)
{
    qDebug() << "Remove me from connections list" << uuid;
    fConnections.remove(uuid);
}
