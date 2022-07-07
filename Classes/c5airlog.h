#ifndef C5AIRLOG_H
#define C5AIRLOG_H

#include <QObject>

#define LOG_WAITER 48

class C5Airlog : public QObject
{
    Q_OBJECT
public:
    explicit C5Airlog(QObject *parent = nullptr);
    static void write(const QString &host, const QString &user, int type, const QString &rec, const QString &invoice, const QString &reservation, const QString &action, const QString &v1, const QString &v2);

private:
    QString fHost;
    QString fUser;
    int fType;
    QString fRec;
    QString fInvoice;
    QString fReservation;
    QString fAction;
    QString fV1;
    QString fV2;

private slots:
    void writeToDatabase();

signals:
    void finished();

};

#endif // C5AIRLOG_H
