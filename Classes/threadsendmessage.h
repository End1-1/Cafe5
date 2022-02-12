#ifndef THREADSENDMESSAGE_H
#define THREADSENDMESSAGE_H

#include <QObject>
#include <QSslSocket>

class ThreadSendMessage : public QObject
{
    Q_OBJECT
public:
    explicit ThreadSendMessage(QObject *parent = nullptr);

    void send(int recipient, const QString &message);

protected slots:
    void err(QAbstractSocket::SocketError e);

signals:

};

#endif // THREADSENDMESSAGE_H
