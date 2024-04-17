#ifndef WORKER_H
#define WORKER_H

#include "rawmessage.h"

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

    QHash<QString, QString> fDbConnectionSettings;

private slots:
    void tasks();

    void broadcastForClients(QByteArray data);

    void removeMeFromConnectionList(QString uuid);

signals:
    void handleData(RawMessage*, const QByteArray);

    void removeSocketHolder(SslSocket *);

    void sendToAllClients(QByteArray);

};

#endif // WORKER_H
