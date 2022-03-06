#include "rawdataexchange.h"
#include "database.h"
#include "databaseconnectionmanager.h"
#include "logwriter.h"
#include <random>
#include <QCryptographicHash>

RawDataExchange *RawDataExchange::fInstance = nullptr;
QMutex fMutexTokenUser;

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

void RawDataExchange::registerFirebaseToken(const QString &t, SslSocket *s)
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
}

bool RawDataExchange::createRegisterPhoneRequest(const QString &phone, QString &confirmationCode, SslSocket *s)
{
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(100000, 999999); // define the range
    confirmationCode = QString::number(distr(gen));

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
        return false;
    }
    return true;
}

bool RawDataExchange::checkPhoneSMSRequest(const QString &sms, SslSocket *s)
{
    QString token = fMapSocketToken[s];
    Database db;
    if (DatabaseConnectionManager::openSystemDatabase(db)) {
        db[":fstate"] = 1;
        db[":fconfirmation_code"] = sms;
        db[":ftoken"] = token;
        if (!db.exec("select * from users_registration where fstate=:fstate and fconfirmation_code=:fconfirmation_code and ftoken=:ftoken")) {
            return false;
        }
        if (!db.next()) {
            return false;
        }
        QString phone = db.stringValue("fphone");
        db[":fid"] = db.integerValue("fid");
        db.exec("update users_registration set fstate=2 where fid=:fid");

        int id = 0;
        db[":fphone"] = phone;
        if (!db.exec("select * from users_list where fphone=:fphone")) {
            return false;
        }
        if (db.next()) {
            id = db.integerValue("fid");
        }

        db[":fgroup"] = 2; // CHECK WHAT THE FUCKING GROUPS
        db[":fphone"] = phone;
        db[":femail"] = "";
        QString password = QCryptographicHash::hash(token.toLocal8Bit(), QCryptographicHash::Md5).toHex();
        db[":fpassword"] = password;
        if (id == 0) {
            if (!db.insert("users_list", id)) {
                return false;
            }
        } else {
            if (!db.update("users_list", "fid", id)) {
                return false;
            }
        }
        db[":ftoken"] = token;
        db[":fuser"] = id;
        if (!db.exec("update users_devices set fuser=:fuser where ftoken=:ftoken")) {
            return false;
        }
        QMutexLocker ml(&fMutexTokenUser);
        fMapTokenUser[token] = id;
    } else {
        LogWriter::write(LogWriterLevel::errors, "RawDataExchange::checkPhoneSMSRequest", db.lastDbError());
        return false;
    }
    return true;
}

bool RawDataExchange::silentAuth(const QString &phone, const QString &password)
{
    Database db;
    if (DatabaseConnectionManager::openSystemDatabase(db)) {
        db[":fphone"] = phone;
        db[":fpassword"] = QString(QCryptographicHash::hash(password.toLocal8Bit(), QCryptographicHash::Md5).toHex());
        if (!db.exec("select fid from users_list where fphone=:fphone and fpassword=:fpassword")) {
            LogWriter::write(LogWriterLevel::errors, "RawDataExchange::balanceAmount", db.lastDbError());
            return false;
        }
        if (db.next()) {
            return true;
        } else {
            return false;
        }
    } else {
        LogWriter::write(LogWriterLevel::errors, "RawDataExchange::balanceAmount", db.lastDbError());
        return false;
    }
    return false;
}

bool RawDataExchange::balanceAmount(SslSocket *s, double &amount)
{
    amount = 0;
    int user = fMapTokenUser[fMapSocketToken[s]];
    qDebug() << user << "request balance amount";
    Database db;
    if (DatabaseConnectionManager::openSystemDatabase(db)) {
        if (!db.exec("select sum(finout*famount) as famount from acc_balance ")) {
            LogWriter::write(LogWriterLevel::errors, "RawDataExchange::balanceAmount", db.lastDbError());
            return false;
        }
        if (db.next()) {
            amount = db.doubleValue("famount");
        }
    } else {
        LogWriter::write(LogWriterLevel::errors, "RawDataExchange::balanceAmount", db.lastDbError());
        return false;
    }
    return true;
}
