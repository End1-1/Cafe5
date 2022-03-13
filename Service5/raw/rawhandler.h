#ifndef RAWHANDLER_H
#define RAWHANDLER_H

#include <QObject>

class SslSocket;

class RawHandler : public QObject
{
    Q_OBJECT
public:
    explicit RawHandler(SslSocket *socket, const QString &session);
    ~RawHandler();
    void run(quint32 msgNum, quint32 msgId, qint16 msgType, const QByteArray &data);

private:
    SslSocket *fSocket;
    quint32 fMsgNum;
    quint32 fMsgId;
    quint16 fMsgType;

signals:
    void writeToSocket(const QByteArray &);

};

#endif // RAWHANDLER_H
