#include "rawsilentauth.h"
#include "databaseconnectionmanager.h"
#include "database.h"
#include "logwriter.h"
#include "messagelist.h"

RawSilentAuth::RawSilentAuth(SslSocket *s) :
    Raw(s)
{

}

RawSilentAuth::~RawSilentAuth()
{
    qDebug() << "~RawSilentAuth";
}

void RawSilentAuth::run(const QByteArray &d)
{
    QString phone, password;
    readString(phone, d);
    readString(password, d);
    Database db;
    quint8 reply = 0;
    if (DatabaseConnectionManager::openSystemDatabase(db)) {
        db[":fphone"] = phone;
        db[":fpassword"] = QString(QCryptographicHash::hash(password.toLocal8Bit(), QCryptographicHash::Md5).toHex());
        if (db.exec("select fid from users_list where fphone=:fphone and fpassword=:fpassword")) {
            if (db.next()) {
                reply = 1;
                const QString &token = tokenOfSocket(fSocket);
                fMapTokenUser[token] = db.integerValue("fid");
                ConnectionStatus cs;
                cs.user = db.integerValue("fid");
                cs.authorized = true;
                cs.online = true;
                CoordinateData cd;
                cd.azimuth = 0;
                cd.latitude = 0;
                cd.longitude = 0;
                cd.speed = 0;
                cd.datetime = 0;
                cs.lastCoordinate = cd;
                fMapTokenConnectionStatus[token] = cs;

                RawMessage r(nullptr);
                r.setHeader(0, 0, MessageList::srv_connections_count);
                r.putUInt(fMapTokenSocket.count());
                r.putUInt(cs.user);
                r.putUByte(1);
                informMonitors(r.data());
            }
        } else {
            LogWriter::write(LogWriterLevel::errors, "RawSilenthAuth::balanceAmount", db.lastDbError());
        }
    } else {
        LogWriter::write(LogWriterLevel::errors, "RawSilenthAuth::balanceAmount", db.lastDbError());
    }
    putUByte(reply);
}
