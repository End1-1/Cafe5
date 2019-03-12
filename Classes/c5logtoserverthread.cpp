#include "c5logtoserverthread.h"
#include "c5sockethandler.h"
#include "c5utils.h"
#include "c5config.h"
#include <QMutex>

QMutex fLogThreadMutex;

C5LogToServerThread::C5LogToServerThread(QObject *parent) :
    QThread(parent)
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void C5LogToServerThread::remember(int type, int user, const QString &rec, const QString &invoice, const QString &reservation, const QString &action, const QString &value1, const QString &value2)
{
    C5LogToServerThread *l = new C5LogToServerThread(__c5config.fParentWidget);
    l->fType = type;
    l->fUser = user;
    l->fRec = rec;
    l->fInvoice = invoice;
    l->fReservation = reservation;
    l->fAction = action;
    l->fValue1 = value1;
    l->fValue2 = value2;
    l->start();
}

void C5LogToServerThread::run()
{
    QMutexLocker ml(&fLogThreadMutex);
    msleep(200);
    C5SocketHandler *s = new C5SocketHandler(0, __c5config.fParentWidget);
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
}
