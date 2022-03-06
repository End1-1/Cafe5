#include "rawhandler.h"
#include "messagelist.h"
#include "rawcoordinate.h"
#include "rawhello.h"
#include "rawyandexkey.h"
#include "rawregisterphone.h"
#include "rawregistersms.h"
#include "rawbalancehistory.h"
#include "rawsilentauth.h"
#include "sslsocket.h"
#include "logwriter.h"
#include "thread.h"

RawHandler::RawHandler(SslSocket *socket, const QString &session, quint32 msgNum, quint32 msgId, qint16 msgType, const QByteArray &data) :
    QObject(),
    fSocket(socket)
{
    Raw *r = nullptr;
    switch (msgType) {
    case MessageList::hello:
        r = new RawHello(socket, data);
        break;
    case MessageList::coordinate:
        r = new RawCoordinate(socket, data);
        break;
    case MessageList::yandex_map_key:
        r = new RawYandexKey(socket, data);
        break;
    case MessageList::silent_auth:
        r = new RawSilentAuth(socket, data);
        break;
    case MessageList::register_phone:
        r = new RawRegisterPhone(socket, data);
        break;
    case MessageList::register_sms:
        r = new RawRegisterSMS(socket, data);
        break;
    case MessageList::balance_history:
        r = new RawBalanceHistory(socket, data);
        break;
    default:
        LogWriter::write(LogWriterLevel::errors, session, QString("Unknown raw command received: %1").arg(msgType));
        fSocket->close();
        return;
    }
    r->setHeader(msgNum, msgId, msgType);
    connect(r, &Raw::finish, this, &RawHandler::writeReply);
    auto *t = new Thread();
    connect(t, &QThread::started, r, &Raw::run);
    connect(t, &QThread::finished, t, &QThread::deleteLater);
    connect(r, &Raw::finish, t, &Thread::quit);
    r->moveToThread(t);
    t->start();
}

RawHandler::~RawHandler()
{
    qDebug() << "~RawHandler()";
}

void RawHandler::writeReply()
{
    fSocket->write(static_cast<Raw*>(sender())->data());
    fSocket->flush();
    sender()->deleteLater();
    deleteLater();
}
