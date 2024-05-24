#include "c5airlog.h"
#include "c5database.h"
#include "c5config.h"
#include <QThread>

C5Airlog::C5Airlog(QObject *parent)
    : QObject(parent)
{
}

void C5Airlog::write(const QString &host, const QString &user, int type,
                     const QString &rec, const QString &invoice, const QString &reservation,
                     const QString &action, const QString &v1, const QString &v2)
{
    auto *a = new C5Airlog();
    a->fHost = host;
    a->fUser =  user;
    a->fType = type;
    a->fRec = rec;
    a->fInvoice = invoice;
    a->fReservation = reservation;
    a->fAction = action;
    a->fV1 = v1;
    a->fV2 = v2;
    auto *t = new QThread();
    t->connect(t, &QThread::started, a, &C5Airlog::writeToDatabase);
    t->connect(t, &QThread::finished, t, &QThread::deleteLater);
    a->connect(a, &C5Airlog::finished, a, &C5Airlog::deleteLater);
    a->moveToThread(t);
    t->start();
}

void C5Airlog::writeToDatabase()
{
    C5Database db(C5Config::dbParams().at(0), C5Config::logDatabase(), C5Config::dbParams().at(2),
                  C5Config::dbParams().at(3));
    db[":f_comp"] = fHost;
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db[":f_user"] = fUser;
    db[":f_type"] = fType;
    db[":f_rec"] = fRec;
    db[":f_invoice"] = fInvoice;
    db[":f_reservation"] = fReservation;
    db[":f_action"] = fAction;
    db[":f_value1"] = fV1;
    db[":f_value2"] = fV2;
    db.insert("airlog.log", false);
    emit finished();
}
