#include "rawhandler.h"
#include "messagelist.h"
#include "rawcoordinate.h"
#include "rawhello.h"
#include "rawyandexkey.h"
#include "rawregisterphone.h"
#include "rawregistersms.h"
#include "rawbalancehistory.h"
#include "rawdllop.h"
#include "rawsilentauth.h"
#include "rawmonitor.h"
#include "rawcarnear.h"
#include "sslsocket.h"
#include "logwriter.h"
#include "thread.h"
#include "socketthread.h"

RawHandler::RawHandler(SslSocket *socket, const QString &session) :
    QObject(),
    fSocket(socket)
{
    setProperty("session", session);
}

RawHandler::~RawHandler()
{
    qDebug() << "~RawHandler()";
}

int RawHandler::run(quint32 msgNum, quint32 msgId, qint16 msgType, const QByteArray &data)
{
//    if (msgType != MessageList::hello) {
//        return;
//    }
    fMsgNum = msgNum;
    fMsgId = msgId;
    fMsgType = msgType;

    Raw *r = nullptr;
    switch (fMsgType) {
    case MessageList::hello:
        r = new RawHello(fSocket);
        break;
    case MessageList::coordinate:
        r = new RawCoordinate(fSocket);
        break;
    case MessageList::yandex_map_key:
        r = new RawYandexKey(fSocket);
        break;
    case MessageList::silent_auth:
        r = new RawSilentAuth(fSocket);
        break;
    case MessageList::register_phone:
        r = new RawRegisterPhone(fSocket);
        break;
    case MessageList::register_sms:
        r = new RawRegisterSMS(fSocket);
        break;
    case MessageList::balance_history:
        r = new RawBalanceHistory(fSocket);
        break;
    case MessageList::monitor:
        r = new RawMonitor(fSocket);
        break;
    case MessageList::car_near:
        r = new RawCarNear(fSocket);
        break;
    case MessageList::dll_op:
        r = new RawDllOp(fSocket);
        break;
    default:
        LogWriter::write(LogWriterLevel::errors, property("session").toString(), QString("Unknown raw command received: %1").arg(msgType));
        fSocket->close();
        return 0;
    }
    r->setProperty("session", property("session"));
    int result = r->run(data);
    r->setHeader(fMsgNum, fMsgId, fMsgType);
    emit writeToSocket(r->data());
    r->deleteLater();
    return result;
}
