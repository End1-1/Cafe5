#ifndef RAWDATAEXCHANGE_H
#define RAWDATAEXCHANGE_H

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
    void registerFirebaseToken(const QString &t, SslSocket *s);
    bool createRegisterPhoneRequest(const QString &phone, QString &confirmationCode, SslSocket *s);
    bool checkPhoneSMSRequest(const QString &sms, SslSocket *s);
    bool silentAuth(const QString &phone, const QString &password);
    bool balanceAmount(SslSocket *s, double &amount);

private:
    static RawDataExchange *fInstance;
    QHash<QString, int> fMapTokenUser;
    QHash<SslSocket*, QString> fMapSocketToken;
    QHash<QString, SslSocket*> fMapTokenSocket;
};

#endif // RAWDATAEXCHANGE_H
