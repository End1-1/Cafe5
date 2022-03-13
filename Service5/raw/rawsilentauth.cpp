#include "rawsilentauth.h"
#include "databaseconnectionmanager.h"
#include "database.h"
#include "logwriter.h"

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
    QString phone = readString(d);
    QString password = readString(d);
    Database db;
    quint8 reply = 0;
    if (DatabaseConnectionManager::openSystemDatabase(db)) {
        db[":fphone"] = phone;
        db[":fpassword"] = QString(QCryptographicHash::hash(password.toLocal8Bit(), QCryptographicHash::Md5).toHex());
        if (db.exec("select fid from users_list where fphone=:fphone and fpassword=:fpassword")) {
            if (db.next()) {
                reply = 1;
                fMapTokenUser[fMapSocketToken[fSocket]] = db.integerValue("fid");
            }
        } else {
            LogWriter::write(LogWriterLevel::errors, "RawSilenthAuth::balanceAmount", db.lastDbError());
        }
    } else {
        LogWriter::write(LogWriterLevel::errors, "RawSilenthAuth::balanceAmount", db.lastDbError());
    }
    putUByte(reply);
}
