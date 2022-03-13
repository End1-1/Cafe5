#ifndef RAWMESSAGE_H
#define RAWMESSAGE_H

#include "sslsocket.h"
#include <QObject>
#include <QDebug>

class RawMessage : public QObject
{
    Q_OBJECT
public:
    explicit RawMessage(SslSocket *s);
    ~RawMessage();
    inline SslSocket *socket() {return fSocket;}
    void setHeader(quint32 msgNum, quint32 msgId, quint16 msgType);
    static void setHeader(QByteArray &d, quint32 msgNum, quint32 msgId, quint16 msgType);
    void setPacketNumber(quint32 n);
    static void setPacketNumber(QByteArray &d, quint32 n);
    QByteArray &data();
    void clearData();
    void putUByte(quint8 v);
    void putUShort(quint16 v);
    void putUInt(quint32 v);
    void putDouble(double v);
    void putString(const QString &v);
    void putBytes(const char *data, quint32 size);
    quint8 readUByte(const QByteArray &d);
    quint32 readUInt(const QByteArray &d);
    const QString readString(const QByteArray &d);
    void readBytes(char *buf, const QByteArray &d);
    void clear();

protected:
    SslSocket *fSocket;
    QByteArray fReply;
    void setDataSize(quint32 sz);

private:
    quint32 getDataSize();
    void increaseDataSize(quint32 sz);
    quint32 fDataPosition;

};

#endif // RAWMESSAGE_H
