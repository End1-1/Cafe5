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
    void setHeader(quint32 msgPacketNum, quint32 msgId, quint16 msgType);
    QByteArray header() const;
    static void setHeader(QByteArray &d, quint32 msgPacketNum, quint32 msgId, quint16 msgType);
    void setPacketNumber(quint32 n);
    static void setPacketNumber(QByteArray &d, quint32 n);
    QByteArray &data();
    void clearData();
    void putUByte(quint8 v);
    void putUShort(quint16 v);
    int putUInt(quint32 v);
    int putDouble(double v);
    int putString(const QString &v);
    void putBytes(const char *data, quint32 size);
    void readUByte(quint8 &i, const QByteArray &d);
    void readUShort(quint16 &i, const QByteArray &d);
    void readUInt(quint32 &i, const QByteArray &d);
    void readDouble(double &v, const QByteArray &d);
    void readString(QString &s, const QByteArray &d);
    void readBytes(char *buf, const QByteArray &d);
    void clear();
    void clear(const QByteArray &header);
    int getPosition();
    void setPosition(int pos);
    void writeToSocket();
    quint32 fDataPosition;
    QByteArray fReply;

protected:
    SslSocket *fSocket;
    void setDataSize(quint32 sz);

private:
    quint32 getDataSize();
    void increaseDataSize(quint32 sz);

};

#endif // RAWMESSAGE_H
