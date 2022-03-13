#include "rawregistersms.h"
#include "database.h"
#include "databaseconnectionmanager.h"
#include "logwriter.h"

RawRegisterSMS::RawRegisterSMS(SslSocket *s) :
    Raw(s)
{

}

RawRegisterSMS::~RawRegisterSMS()
{
    qDebug() << "~RawRegisterSMS";
}

void RawRegisterSMS::run(const QByteArray &d)
{
    QString sms;
    readString(sms, d);
    quint16 reply = 0;
    putUShort(reply);
    QString token = fMapSocketToken[fSocket];
    Database db;
    if (DatabaseConnectionManager::openSystemDatabase(db)) {
        db[":fstate"] = 1;
        db[":fconfirmation_code"] = sms;
        db[":ftoken"] = token;
        if (db.exec("select * from users_registration where fstate=:fstate and fconfirmation_code=:fconfirmation_code and ftoken=:ftoken")) {
            if (db.next()) {
                QString phone = db.stringValue("fphone");
                db[":fid"] = db.integerValue("fid");
                db.exec("update users_registration set fstate=2 where fid=:fid");

                int id = 0;
                db[":fphone"] = phone;
                if (db.exec("select * from users_list where fphone=:fphone")) {
                    if (db.next()) {
                        id = db.integerValue("fid");
                    }
                } else {
                    LogWriter::write(LogWriterLevel::errors, property("session").toString(), db.lastDbError());
                    return;
                }

                db[":fgroup"] = 2; // CHECK WHAT THE FUCKING GROUPS
                db[":fphone"] = phone;
                db[":femail"] = "";
                QString password = QCryptographicHash::hash(token.toLocal8Bit(), QCryptographicHash::Md5).toHex();
                db[":fpassword"] = password;
                if (id == 0) {
                    if (!db.insert("users_list", id)) {
                        LogWriter::write(LogWriterLevel::errors, property("session").toString(), db.lastDbError());
                        return;
                    }
                } else {
                    if (!db.update("users_list", "fid", id)) {
                        LogWriter::write(LogWriterLevel::errors, property("session").toString(), db.lastDbError());
                        return;
                    }
                }
                db[":ftoken"] = token;
                db[":fuser"] = id;
                if (!db.exec("update users_devices set fuser=:fuser where ftoken=:ftoken")) {
                    LogWriter::write(LogWriterLevel::errors, property("session").toString(), db.lastDbError());
                    return;
                }
                QMutexLocker ml(fMutexTokenUser);
                fMapTokenUser[token] = id;
                reply = 1;
            }
        }
    } else {
        LogWriter::write(LogWriterLevel::errors, "RawRegisterSMS::checkPhoneSMSRequest", db.lastDbError());
    }
    clearData();
    putUShort(reply);
}
