#include "worker.h"
#include "logwriter.h"
#include <QThread>
#include <QTimer>

WaiterClientHandler::WaiterClientHandler(QObject *parent) :
    QObject(parent),
    fTaskCounter(0)
{
    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &WaiterClientHandler::tasks);
#ifdef QT_DEBUG
    timer->start(10000);
#else
    timer->start(60000);
#endif
}

WaiterClientHandler::~WaiterClientHandler()
{
    qDebug() << "~WaiterClientHandler()";
}

void WaiterClientHandler::run(RawMessage *rm, const QByteArray &in)
{

    emit handleData(rm, in);
}

void WaiterClientHandler::removeSocket(SslSocket *s)
{
    emit removeSocketHolder(s);
}

void WaiterClientHandler::tasks()
{
    fTaskCounter++;
    LogWriter::write(LogWriterLevel::verbose, "", "Syncfromaws started");
}

void WaiterClientHandler::broadcastForClients(QByteArray data)
{
    qDebug() << "Broadcast for clients";
    emit sendToAllClients(data);
}

void WaiterClientHandler::removeMeFromConnectionList(QString uuid)
{

}
