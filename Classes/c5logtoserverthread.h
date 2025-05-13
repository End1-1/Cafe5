#ifndef C5LOGTOSERVERTHREAD_H
#define C5LOGTOSERVERTHREAD_H

#include <QDate>
#include <QTime>

#define LOG_WAITER 48

typedef struct {

    int fType;

    QString fUser;

    QDate fDate;

    QTime fTime;

    QString fRec;

    QString fInvoice;

    QString fReservation;

    QString fAction;

    QString fValue1;

    QString fValue2;

} C5LogRecord;

class C5LogToServerThread : public QObject
{
    Q_OBJECT

public:
    C5LogToServerThread(QObject *parent = nullptr);

    static void configureServer(const QString &ip, int port);

    static void remember(int type, const QString &user, const QString &rec, const QString &invoice,
                         const QString &reservation, const QString &action, const QString &value1, const QString &value2);

protected:

    int fType;

    QString fUser;

    QString fRec;

    QString fInvoice;

    QString fReservation;

    QString fAction;

    QString fValue1;

    QString fValue2;

signals:
    void finished();
};

#endif // C5LOGTOSERVERTHREAD_H
