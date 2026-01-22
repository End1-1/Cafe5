#ifndef NDATAPROVIDER_H
#define NDATAPROVIDER_H

#include <QObject>
#include <QNetworkReply>
#include <QJsonObject>

class QNetworkAccessManager;
class QElapsedTimer;

class NDataProvider : public QObject
{
    Q_OBJECT
public:
    explicit NDataProvider(QObject *parent = nullptr);

    ~NDataProvider();

    void getData(const QString &route, const QJsonObject &data);

    void changeTimeout(int value);

    void changeBearer(const QString &bearer);

    void overwriteHost(const QString &protocol, const QString &host, int port);

    static bool mDebug;

    static QString mProtocol;

    static QString mHost;

    static QString sessionKey;

    static QString mAppName;

    static QString mFileVersion;

private:
    QElapsedTimer* mTimer;

    QNetworkAccessManager* mNetworkAccessManager;

    QString mConnectionProtocol;

    QString mConnectionHost;

    int mConnectionPort;

    QString mBearer;

    QDateTime mStartDate;

private slots:
    void queryFinished(QNetworkReply *r);

signals:
    void started();

    void error(const QString&);

    void done(const QJsonObject&);

    void updateRequired(const QString&, const QString&, const QString&);
};

#endif // NDATAPROVIDER_H
