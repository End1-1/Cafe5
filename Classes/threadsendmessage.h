#ifndef THREADSENDMESSAGE_H
#define THREADSENDMESSAGE_H

#include <QObject>
#include <QSslSocket>
#include <QJsonObject>

class ThreadSendMessage : public QObject
{
    Q_OBJECT
public:
    explicit ThreadSendMessage(QObject *parent = nullptr);

    void send(int recipient, const QString &message);

protected slots:
    void start();

    void err(QAbstractSocket::SocketError e);

private:
    int fRecipient;

    QString fMessage;

signals:
    void finished();

    void result(const QJsonObject &);

};

#endif // THREADSENDMESSAGE_H
