#ifndef RAWHANDLER_H
#define RAWHANDLER_H

#include <QObject>

class SslSocket;

class RawHandler : public QObject
{
    Q_OBJECT
public:
    explicit RawHandler(SslSocket *socket, const QString &session, quint32 msgNum, quint32 msgId, qint16 msgType, const QByteArray &data);
    ~RawHandler();

private:
    SslSocket *fSocket;

private slots:
    void writeReply();

};

#endif // RAWHANDLER_H
