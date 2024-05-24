#ifndef NDATAPROVIDER_H
#define NDATAPROVIDER_H

#include <QObject>
#include <QNetworkReply>

class QNetworkAccessManager;
//class QNetworkReply;
class QElapsedTimer;

class NDataProvider : public QObject
{
    Q_OBJECT
public:
    explicit NDataProvider(const QString &host, QObject *parent = nullptr);
    ~NDataProvider();
    void getData(const QString &route, const QJsonObject &data);
    void changeTimeout(int value);
    static bool mDebug;
    static QString sessionKey;

private:
    QString mHost;
    QElapsedTimer *mTimer;
    QNetworkAccessManager *mNetworkAccessManager;

private slots:
    void queryFinished(QNetworkReply *r);

signals:
    void started();
    void error(const QString &);
    void done(const QJsonObject &);
};

#endif // NDATAPROVIDER_H
