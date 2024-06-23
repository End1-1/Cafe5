#ifndef WORKER_H
#define WORKER_H

#include "rawmessage.h"
#include <QJsonArray>
#include <QJsonObject>

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

    void getLastId();

    void clearTask();

private slots:
    void tasks();

    void getLastIDResponse(const QJsonObject &jdoc);

    void getDataResponse(const QJsonObject &jdoc);

    void updateLocalResponse(const QJsonObject &jdoc);

    void taskFinish(const QJsonObject &ba);

    void broadcastForClients(QByteArray data);

    void removeMeFromConnectionList(QString uuid);

    void getSyncError(const QString &err);

signals:
    void handleData(RawMessage *, const QByteArray);

    void removeSocketHolder(SslSocket *);

    void sendToAllClients(QByteArray);

};

#endif // WORKER_H
