#ifndef RAWDATAEXCHANGE_H
#define RAWDATAEXCHANGE_H

#include "structs.h"
#include <QObject>
#include <QHash>

class SslSocket;

class RawDataExchange : public QObject
{
    Q_OBJECT
public:
    explicit RawDataExchange(QObject *parent = nullptr);
    static void init();
    static RawDataExchange *instance();
    const QString &tokenOfSocket(SslSocket *s);

public slots:
    void removeSocket();
    void registerFirebaseToken(SslSocket *s, const QString &t);
    void registerPhone(SslSocket *s, const QString &phone);
    void registerSms(SslSocket *s, const QString &sms);
    void registerMonitor(SslSocket *s);
    void silentAuth(SslSocket *s, const QString &phone, const QString &password);
    void balanceAmountTotal(SslSocket *s);
    void devicePosition(SslSocket *s, const CoordinateData &c);

private:
    static RawDataExchange *fInstance;
    QHash<QString, int> fMapTokenUser;
    QHash<SslSocket*, QString> fMapSocketToken;
    QHash<QString, SslSocket*> fMapTokenSocket;
    QList<SslSocket*> fMonitors;
    void informMonitors(const QByteArray &d);

signals:
    void socketData(SslSocket *, const QByteArray &);
    void firebaseMessage(const QString &, const QString &);
};

#endif // RAWDATAEXCHANGE_H
