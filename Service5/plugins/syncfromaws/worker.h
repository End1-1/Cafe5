#ifndef WORKER_H
#define WORKER_H

#include "rawmessage.h"
#include <QJsonArray>
#include <QJsonObject>

class QNetworkAccessManager;
class QNetworkReply;

class WaiterClientHandler : public QObject
{
    Q_OBJECT
public:
    explicit WaiterClientHandler(QObject *parent = nullptr);

    ~WaiterClientHandler();

    void run(RawMessage *rm, const QByteArray &in);

    void removeSocket(SslSocket *s);

private:
    int fTaskCounter;

    bool mTaskRunning;

    QJsonArray mServers;

    QJsonObject mLocal;

    QJsonObject mRemote;

    QJsonArray mSyncArrary;

    QHash<QString, QString> fDbConnectionSettings;

    QNetworkAccessManager *mNetworkAccessManager;

    void clearTask();

private slots:
    void tasks();

    void taskFinish(const QJsonObject &ba);

    void broadcastForClients(QByteArray data);

    void removeMeFromConnectionList(QString uuid);

    void finished(QNetworkReply *r);

    void getSyncError(const QString &err);

    void getSyncObject(const QJsonObject &ba);

    void getLastTime(const QJsonObject &ba);

signals:
    void handleData(RawMessage *, const QByteArray);

    void removeSocketHolder(SslSocket *);

    void sendToAllClients(QByteArray);

};

#endif // WORKER_H
