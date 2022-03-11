#include "rawdataexchange.h"
#include "database.h"
#include "databaseconnectionmanager.h"
#include "logwriter.h"
#include "sslsocket.h"
#include "rawmessage.h"
#include "socketthread.h"
#include "firebase.h"
#include "messagelist.h"
#include <random>
#include <QCryptographicHash>

RawDataExchange *RawDataExchange::fInstance = nullptr;
QMutex fMutexTokenUser;
QMutex fMutexInformMonitors;

RawDataExchange::RawDataExchange(QObject *parent) :
    QObject(parent)
{
    Database db;
    if (DatabaseConnectionManager::openSystemDatabase(db)) {
        db.exec("select * from users_devices");
        while (db.next()) {
            fMapTokenUser[db.stringValue("ftoken")] = db.integerValue("fuser");
        }
    } else {
        LogWriter::write(LogWriterLevel::errors, "", db.lastDbError());
    }
    connect(this, &RawDataExchange::firebaseMessage, Firebase::instance(), &Firebase::sendMessage);
}

void RawDataExchange::init()
{
    fInstance = new RawDataExchange();
}

RawDataExchange *RawDataExchange::instance()
{
    return fInstance;
}

const QString &RawDataExchange::tokenOfSocket(SslSocket *s)
{
    return fMapSocketToken[s];
}

void RawDataExchange::registerFirebaseToken(SslSocket *s, const QString &t)
{
    if (fMapTokenUser.contains(t)) {
    } else {
        Database db;
        if (DatabaseConnectionManager::openSystemDatabase(db)) {
            db[":ftoken"] = t;
            db[":fuser"] = 0;
            db.insert("users_devices");
            QMutexLocker ml(&fMutexTokenUser);
            fMapTokenUser[t] = 0;
        } else {
            LogWriter::write(LogWriterLevel::errors, "RawDataExchange::registerFirebaseToken", db.lastDbError());
        }
    }
    QMutexLocker ml(&fMutexTokenUser);
    fMapTokenSocket[t] = s;
    fMapSocketToken[s] = t;

    RawMessage r(s, QByteArray());
    r.setHeader(0, 0, MessageList::srv_connections_count);
    r.putUInt(fMapSocketToken.count());
    informMonitors(r.data());
}

void RawDataExchange::registerPhone(SslSocket *s, const QString &phone)
{
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(100000, 999999); // define the range
    QString confirmationCode = QString::number(distr(gen));

    Database db;
    if (DatabaseConnectionManager::openSystemDatabase(db)) {
        db[":fstate"] = 3;
        db[":fphone"] = phone;
        db.exec("update users_registration set fstate=:fstate where fstate=1 and fphone=:fphone");
        db[":fstate"] = 1;
        db[":fphone"] = phone;
        db[":fconfirmation_code"] = confirmationCode;
        db[":fcreatedate"] = QDate::currentDate();
        db[":fcreatetime"] = QTime::currentTime();
        db[":ftoken"] = fMapSocketToken[s];
        db.insert("users_registration");
    } else {
        LogWriter::write(LogWriterLevel::errors, "RawDataExchange::createRegisterPhoneRequest", db.lastDbError());
    }
    quint16 reply = 0;
    RawMessage r(nullptr, QByteArray());
    r.putUShort(reply);
    emit socketData(s, r.data());
    emit firebaseMessage(tokenOfSocket(s), QString("Your confirmation code: %1").arg(confirmationCode));
}

void RawDataExchange::registerSms(SslSocket *s, const QString &sms)
{
    quint16 reply = 0;
    RawMessage r(s, QByteArray());
    r.putUShort(reply);
    QString token = fMapSocketToken[s];
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
                    LogWriter::write(LogWriterLevel::errors, s->property("session").toString(), db.lastDbError());
                    emit socketData(s, r.data());
                    return;
                }

                db[":fgroup"] = 2; // CHECK WHAT THE FUCKING GROUPS
                db[":fphone"] = phone;
                db[":femail"] = "";
                QString password = QCryptographicHash::hash(token.toLocal8Bit(), QCryptographicHash::Md5).toHex();
                db[":fpassword"] = password;
                if (id == 0) {
                    if (!db.insert("users_list", id)) {
                        LogWriter::write(LogWriterLevel::errors, s->property("session").toString(), db.lastDbError());
                        emit socketData(s, r.data());
                        return;
                    }
                } else {
                    if (!db.update("users_list", "fid", id)) {
                        LogWriter::write(LogWriterLevel::errors, s->property("session").toString(), db.lastDbError());
                        emit socketData(s, r.data());
                        return;
                    }
                }
                db[":ftoken"] = token;
                db[":fuser"] = id;
                if (!db.exec("update users_devices set fuser=:fuser where ftoken=:ftoken")) {
                    LogWriter::write(LogWriterLevel::errors, s->property("session").toString(), db.lastDbError());
                    emit socketData(s, r.data());
                    return;
                }
                QMutexLocker ml(&fMutexTokenUser);
                fMapTokenUser[token] = id;
                reply = 1;
            }
        }


    } else {
        LogWriter::write(LogWriterLevel::errors, "RawDataExchange::checkPhoneSMSRequest", db.lastDbError());
    }
    r.clearData();
    r.putUShort(reply);
    emit socketData(s, r.data());
}

void RawDataExchange::silentAuth(SslSocket *s, const QString &phone, const QString &password)
{
    Database db;
    quint8 reply = 0;
    if (DatabaseConnectionManager::openSystemDatabase(db)) {
        db[":fphone"] = phone;
        db[":fpassword"] = QString(QCryptographicHash::hash(password.toLocal8Bit(), QCryptographicHash::Md5).toHex());
        if (db.exec("select fid from users_list where fphone=:fphone and fpassword=:fpassword")) {
            if (db.next()) {
                reply = 1;
                fMapTokenUser[fMapSocketToken[s]] = db.integerValue("fid");
            }
        } else {
            LogWriter::write(LogWriterLevel::errors, "RawDataExchange::balanceAmount", db.lastDbError());
        }
    } else {
        LogWriter::write(LogWriterLevel::errors, "RawDataExchange::balanceAmount", db.lastDbError());
    }

    RawMessage r(nullptr, QByteArray());
    r.putUByte(reply);
    emit socketData(s, r.data());
}

void RawDataExchange::balanceAmountTotal(SslSocket *s)
{
    double amount = 0;
    int user = fMapTokenUser[fMapSocketToken[s]];
    qDebug() << user << "request balance amount";
    Database db;
    if (DatabaseConnectionManager::openSystemDatabase(db)) {
        if (!db.exec("select sum(finout*famount) as famount from acc_balance ")) {
            LogWriter::write(LogWriterLevel::errors, "RawDataExchange::balanceAmount", db.lastDbError());
        }
        if (db.next()) {
            amount = db.doubleValue("famount");
        }
    } else {
        LogWriter::write(LogWriterLevel::errors, "RawDataExchange::balanceAmount", db.lastDbError());
    }
    quint8 reply = 1;
    RawMessage r(nullptr, QByteArray());
    r.putUByte(reply);
    r.putDouble(amount);
    emit socketData(s, r.data());
}

void RawDataExchange::registerMonitor(SslSocket *s)
{
    QMutexLocker ml(&fMutexInformMonitors);
    fMonitors.append(s);
}

void RawDataExchange::devicePosition(SslSocket *s, const CoordinateData &c)
{
    QString token = fMapSocketToken[s];
    int user = fMapTokenUser[token];
    RawMessage r(nullptr, QByteArray());
    r.setHeader(0, 0, MessageList::srv_device_position);
    r.putUInt(user);
    r.putBytes(reinterpret_cast<const char*>(&c), sizeof(c));
    informMonitors(r.data());
    r.clear();
    emit socketData(s, r.data());
}

void RawDataExchange::informMonitors(const QByteArray &d)
{
    //QMutexLocker ml(&fMutexInformMonitors);
    for (SslSocket *s: qAsConst(fMonitors)) {
        s->write(d);
    }
}

void RawDataExchange::removeSocket()
{
    auto *st = static_cast<SocketThread*>(sender());
    auto *s = st->fSslSocket;
    fMonitors.removeAll(s);
    fMapTokenSocket.remove(fMapSocketToken[s]);
    fMapSocketToken.remove(s);
    sender()->deleteLater();

    RawMessage r(s, QByteArray());
    r.setHeader(0, 0, MessageList::srv_connections_count);
    r.putUInt(fMapTokenSocket.count());
    informMonitors(r.data());
}
