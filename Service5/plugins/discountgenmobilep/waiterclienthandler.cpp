#include "waiterclienthandler.h"
#include <QThread>
#include <QTimer>

WaiterClientHandler::WaiterClientHandler(QHash<QString, QString> conn, QObject *parent) :
    QObject(parent),
    fDbConnectionSettings(conn),
    fTaskCounter(0)
{
    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &WaiterClientHandler::tasks);
    timer->start(1000);
}

WaiterClientHandler::~WaiterClientHandler()
{
    qDebug() << "~WaiterClientHandler()";
}

void WaiterClientHandler::run(RawMessage *rm, const QByteArray &in)
{
    if (!fConnections.contains(rm->socket()->fUuid)) {
        qDebug() << "Created new plugin connection" << rm->socket()->fUuid;
        auto *w = new WaiterConnection(fDbConnectionSettings, rm->socket());
        connect(this, &WaiterClientHandler::handleData, w, &WaiterConnection::handleData);
        connect(this, &WaiterClientHandler::removeSocketHolder, w, &WaiterConnection::removeSocket);
        connect(w, &WaiterConnection::broadcastForClients, this, &WaiterClientHandler::broadcastForClients);
        connect(w, &WaiterConnection::removeMeFromConnectionList, this, &WaiterClientHandler::removeMeFromConnectionList);
        connect(this, &WaiterClientHandler::sendToAllClients, w, &WaiterConnection::writeToSocket);
        QThread *t = new QThread();
        connect(w, &WaiterConnection::destroyed, t, &QThread::deleteLater);
        w->moveToThread(t);
        t->start();
        fConnections[rm->socket()->fUuid] = w;
    }
    qDebug() << "emit handledata for" << rm->socket()->fUuid;
    emit handleData(rm, in);
}

void WaiterClientHandler::removeSocket(SslSocket *s)
{
    emit removeSocketHolder(s);
}

void WaiterClientHandler::tasks()
{
    fTaskCounter++;
    if (fTaskCounter %10) {

    }
}

void WaiterClientHandler::broadcastForClients(QByteArray data)
{
    qDebug() << "Broadcast for clients";
    emit sendToAllClients(data);
}

void WaiterClientHandler::removeMeFromConnectionList(QString uuid)
{
    qDebug() << "Remove me from connections list" << uuid;
    fConnections.remove(uuid);
}
