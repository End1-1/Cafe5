#include "rawregisterphone.h"
#include "firebase.h"
#include "database.h"
#include "databaseconnectionmanager.h"
#include "logwriter.h"
#include <random>

RawRegisterPhone::RawRegisterPhone(SslSocket *s) :
    Raw(s)
{
    connect(this, &RawRegisterPhone::firebaseMessage, Firebase::instance(), &Firebase::sendMessage);
}

RawRegisterPhone::~RawRegisterPhone()
{
    qDebug() << "RawRegisterPhone";
}

void RawRegisterPhone::run(const QByteArray &d)
{
    QString phone = readString(d);

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
        db[":ftoken"] = fMapSocketToken[fSocket];
        db.insert("users_registration");
    } else {
        LogWriter::write(LogWriterLevel::errors, "RawRegisterPhone::createRegisterPhoneRequest", db.lastDbError());
    }
    quint16 reply = 0;
    putUShort(reply);
    emit firebaseMessage(tokenOfSocket(fSocket), QString("Your confirmation code: %1").arg(confirmationCode));
}
