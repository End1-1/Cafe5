#include "rawhello.h"
#include "databaseconnectionmanager.h"
#include "logwriter.h"
#include "messagelist.h"

RawHello::RawHello(SslSocket *s) :
    Raw(s)
{
}

RawHello::~RawHello()
{
    qDebug() << "~RawHello";
}

int RawHello::run(const QByteArray &d)
{
    QMutexLocker ml(fMutexTokenUser);
    QString firebaseToken;
    readString(firebaseToken, d);
    int result = 0;
    if (fMapTokenUser.contains(firebaseToken)) {
        if (fMapTokenUser[firebaseToken] > 0) {
            result = 1;
        } else {
            result = 2;
        }
    } else {
        Database db;
        if (DatabaseConnectionManager::openSystemDatabase(db)) {
            db[":ftoken"] = firebaseToken;
            db[":fuser"] = 0;
            db.insert("users_devices");
            fMapTokenUser[firebaseToken] = 0;
            result = 2;
        } else {
            LogWriter::write(LogWriterLevel::errors, "RawHello::registerFirebaseToken", db.lastDbError());
        }
    }
    fMapTokenSocket[firebaseToken] = fSocket;
    fMapSocketToken[fSocket] = firebaseToken;

    setHeader(0, 0, MessageList::srv_connections_count);
    putUInt(fMapSocketToken.count());
    informMonitors(fReply);
    clear();
    return result;
}
