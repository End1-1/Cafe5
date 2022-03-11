#include "rawhandler.h"
#include "messagelist.h"
#include "rawcoordinate.h"
#include "rawhello.h"
#include "rawyandexkey.h"
#include "rawregisterphone.h"
#include "rawregistersms.h"
#include "rawbalancehistory.h"
#include "rawsilentauth.h"
#include "rawmonitor.h"
#include "sslsocket.h"
#include "logwriter.h"
#include "thread.h"

RawHandler::RawHandler(SslSocket *socket, const QString &session) :
    QObject(),
    fSocket(socket)
{

}

RawHandler::~RawHandler()
{
    qDebug() << "~RawHandler()";
}

void RawHandler::run(quint32 msgNum, quint32 msgId, qint16 msgType, const QByteArray &data)
{
    fMsgNum = msgNum;
    fMsgId = msgId;
    fMsgType = msgType;

    Raw *r = nullptr;
    switch (fMsgType) {
    case MessageList::hello:
        r = new RawHello(fSocket, data);
        break;
    case MessageList::coordinate:
        r = new RawCoordinate(fSocket, data);
        break;
    case MessageList::yandex_map_key:
        r = new RawYandexKey(fSocket, data);
        break;
    case MessageList::silent_auth:
        r = new RawSilentAuth(fSocket, data);
        break;
    case MessageList::register_phone:
        r = new RawRegisterPhone(fSocket, data);
        break;
    case MessageList::register_sms:
        r = new RawRegisterSMS(fSocket, data);
        break;
    case MessageList::balance_history:
        r = new RawBalanceHistory(fSocket, data);
        break;
    case MessageList::monitor:
        r = new RawMonitor(fSocket, data);
        break;
    default:
        LogWriter::write(LogWriterLevel::errors, property("session").toString(), QString("Unknown raw command received: %1").arg(msgType));
        fSocket->close();
        return;
    }
    r->setHeader(fMsgNum, fMsgId, fMsgType);
    connect(r, &Raw::finish, this, &RawHandler::writeReply);
    auto *t = new Thread(QString("RawHandler %1").arg(r->metaObject()->className()));
    connect(t, &QThread::started, r, &Raw::run);
    connect(t, &QThread::finished, t, &QThread::deleteLater);
    connect(r, &Raw::destroyed, t, &Thread::quit);
    r->moveToThread(t);
    t->start();
}

void RawHandler::writeReply(QByteArray d)
{
    sender()->deleteLater();
    Raw::setHeader(d, fMsgNum, fMsgId, fMsgType);
    emit writeToSocket(d);
}
