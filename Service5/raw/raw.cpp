#include "raw.h"
#include "databaseconnectionmanager.h"
#include "database.h"
#include "logwriter.h"
#include "messagelist.h"
#include <QMutex>

QHash<QString, int> Raw::fMapTokenUser;
QHash<QString, ConnectionStatus> Raw::fMapTokenConnectionStatus;
QHash<SslSocket*, QString> Raw::fMapSocketToken;
QHash<QString, SslSocket*> Raw::fMapTokenSocket;
QList<SslSocket*> Raw::fMonitors;

QMutex *Raw::fMutexTokenUser = nullptr;
QMutex *Raw::fMutexInformMonitors = nullptr;

Raw::Raw(SslSocket *s) :
    RawMessage(s)
{
}

Raw::~Raw()
{
}

void Raw::informMonitors(const QByteArray &d)
{
    QMutexLocker ml(fMutexInformMonitors);
    for (SslSocket *s: qAsConst(fMonitors)) {
        s->write(d);
    }
}

void Raw::init()
{
    fMutexInformMonitors = new QMutex();
    fMutexTokenUser = new QMutex();
    Database db;
    if (DatabaseConnectionManager::openSystemDatabase(db)) {
        db.exec("select * from users_devices");
        while (db.next()) {
            fMapTokenUser[db.stringValue("ftoken")] = db.integerValue("fuser");
        }
    } else {
        LogWriter::write(LogWriterLevel::errors, "", db.lastDbError());
    }
}

const QString &Raw::tokenOfSocket(SslSocket *s)
{
    return fMapSocketToken[s];
}

void Raw::removeSocket(SslSocket *s)
{
    QMutexLocker ml1(fMutexInformMonitors);
    fMonitors.removeAll(s);
    ml1.unlock();

    QMutexLocker ml2(fMutexTokenUser);
    ConnectionStatus &cs = fMapTokenConnectionStatus[fMapSocketToken[s]];
    cs.online = false;
    fMapTokenSocket.remove(fMapSocketToken[s]);
    fMapSocketToken.remove(s);
    ml2.unlock();

    RawMessage r(nullptr);
    r.setHeader(0, 0, MessageList::srv_connections_count);
    r.putUInt(fMapTokenSocket.count());
    r.putUInt(cs.user);
    r.putUByte(0);
    informMonitors(r.data());
}
