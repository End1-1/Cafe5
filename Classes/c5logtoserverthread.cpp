#include "c5logtoserverthread.h"
#include "c5sockethandler.h"
#include "c5socketmessage.h"
#include "c5utils.h"
#include "c5config.h"
#include <QMutex>

static QMutex fLogThreadMutex;

C5LogToServerThread::C5LogToServerThread(QObject *parent) :
    QObject(parent)
{

}

void C5LogToServerThread::remember(int type, const QString &user, const QString &rec, const QString &invoice, const QString &reservation, const QString &action, const QString &value1, const QString &value2)
{
    C5LogToServerThread *l = new C5LogToServerThread();
    l->fType = type;
    l->fUser = user;
    l->fRec = rec;
    l->fInvoice = invoice;
    l->fReservation = reservation;
    l->fAction = action;
    l->fValue1 = value1;
    l->fValue2 = value2;

    QThread *t = new QThread();
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()));
    connect(t, SIGNAL(started()), l, SLOT(run()));
    connect(l, SIGNAL(finished()), t, SLOT(quit()));
    connect(l, SIGNAL(finished()), l, SLOT(deleteLater()));
    l->moveToThread(t);
    t->start();
}

void C5LogToServerThread::run()
{
    QMutexLocker ml(&fLogThreadMutex);
    C5SocketHandler *s = new C5SocketHandler(nullptr, this);
    s->bind("cmd", sm_log);
    s->bind("comp", hostinfo);
    s->bind("date", QDate::currentDate().toString(FORMAT_DATE_TO_STR_MYSQL));
    s->bind("time", QTime::currentTime().toString(FORMAT_TIME_TO_STR));
    s->bind("type", fType);
    s->bind("user", fUser);
    s->bind("rec", fRec);
    s->bind("invoice", fInvoice);
    s->bind("reservation", fReservation);
    s->bind("action", fAction);
    s->bind("value1", fValue1);
    s->bind("value2", fValue2);
    s->send();
    emit finished();
}
