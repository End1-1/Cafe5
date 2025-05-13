#include "c5logtoserverthread.h"

C5LogToServerThread::C5LogToServerThread(QObject *parent) :
    QObject(parent)
{
}

void C5LogToServerThread::remember(int type, const QString &user, const QString &rec, const QString &invoice,
                                   const QString &reservation, const QString &action, const QString &value1, const QString &value2)
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
}
