#ifndef RAW_H
#define RAW_H

#include <QObject>
#include <QDebug>

class SslSocket;

class Raw : public QObject
{
    Q_OBJECT
public:
    explicit Raw(SslSocket *s, const QByteArray &d);
    void setHeader(quint32 msgNum, quint32 msgId, quint16 msgType);
    const QByteArray &data();

public slots:
    virtual void run() = 0;

protected:
    SslSocket *fSocket;
    const QByteArray fData;
    QByteArray fReply;
    void putUByte(quint8 v);
    void putUShort(quint16 v);
    void putUInt(quint32 v);
    void putDouble(double v);
    void putString(const QString &v);
    quint8 readUByte();
    quint32 readUInt();
    const QString readString();

private:
    quint32 getDataSize();
    void increaseDataSize(quint32 sz);
    void setDataSize(quint32 sz);
    quint32 fDataPosition;

signals:
    void finish();
};

#endif // RAW_H
