#include "rawmessage.h"

RawMessage::RawMessage(SslSocket *s) :
    QObject(nullptr),
    fSocket(s)
{
    clear();
}

RawMessage::~RawMessage()
{
}

void RawMessage::setHeader(quint32 msgPacketNum, quint32 msgId, quint16 msgType)
{
    setHeader(fReply, msgPacketNum, msgId, msgType);
}

QByteArray RawMessage::header() const
{
    return fReply.mid(0, 17);
}

void RawMessage::setHeader(QByteArray &d, quint32 msgPacketNum, quint32 msgId, quint16 msgType)
{
    d.replace(3, 4, reinterpret_cast<const char*>(&msgPacketNum), sizeof(msgPacketNum));
    d.replace(7, 4, reinterpret_cast<const char*>(&msgId), sizeof(msgId));
    d.replace(11, 2, reinterpret_cast<const char*>(&msgType), sizeof(msgType));
}

void RawMessage::setPacketNumber(quint32 n)
{
    setPacketNumber(fReply, n);
}

void RawMessage::setPacketNumber(QByteArray &d, quint32 n)
{
    d.replace(3, 4, reinterpret_cast<const char*>(&n), sizeof(n));
}

QByteArray &RawMessage::data()
{
    return fReply;
}

void RawMessage::clearData()
{
    fReply.remove(17, fReply.length() - 17);
    setDataSize(0);
}

void RawMessage::putUByte(quint8 v)
{
    fReply.append(reinterpret_cast<const char*>(&v), sizeof(quint8));
    increaseDataSize(sizeof(quint8));
}

void RawMessage::setDataSize(quint32 sz)
{
    fReply.replace(13, 4, reinterpret_cast<const char*>(&sz), sizeof(sz));
}

void RawMessage::putUShort(quint16 v)
{
    fReply.append(reinterpret_cast<const char*>(&v), sizeof(quint16));
    increaseDataSize(sizeof(quint16));
}

int RawMessage::putUInt(quint32 v)
{
    fReply.append(reinterpret_cast<const char*>(&v), sizeof(quint32));
    increaseDataSize(sizeof(quint32));
    return fReply.length() - sizeof(quint32);
}

int RawMessage::putDouble(double v)
{
    fReply.append(reinterpret_cast<const char*>(&v), sizeof(double));
    increaseDataSize(sizeof(double));
    return fReply.length() - sizeof(double);
}

int RawMessage::putString(const QString &v)
{
    quint32 sz = v.toUtf8().length() + 1;
    putUInt(sz);
    fReply.append(v.toUtf8());
    fReply.append('\0');
    increaseDataSize(sz);
    return fReply.length() - sz - sizeof(quint32);
}

void RawMessage::putBytes(const char *data, quint32 size)
{
    putUInt(size);
    fReply.append(data, size);
    increaseDataSize(size);
}

void RawMessage::readUByte(quint8 &i, const QByteArray &d)
{
    memcpy(reinterpret_cast<char*>(&i), &d.data()[fDataPosition], sizeof(quint8));
    fDataPosition += sizeof(quint8);
}

void RawMessage::readUShort(quint16 &i, const QByteArray &d)
{
    memcpy(reinterpret_cast<char*>(&i), &d.data()[fDataPosition], sizeof(quint16));
    fDataPosition += sizeof(quint16);
}

void RawMessage::readUInt(quint32 &i, const QByteArray &d)
{
    memcpy(reinterpret_cast<char*>(&i), &d.data()[fDataPosition], sizeof(quint32));
    fDataPosition += sizeof(quint32);
}

void RawMessage::readDouble(double &v, const QByteArray &d)
{
    memcpy(reinterpret_cast<char*>(&v), &d.data()[fDataPosition], sizeof(v));
    fDataPosition += sizeof(v);
}

void RawMessage::readString(QString &s, const QByteArray &d)
{
    quint32 sz;
    readUInt(sz, d);
    fDataPosition += sz;
    s = QString::fromUtf8(d.mid(fDataPosition - sz, sz));
    qDebug() << s;
}

void RawMessage::readBytes(char *buf, const QByteArray &d)
{
    quint32 sz;
    readUInt(sz, d);
    fDataPosition += sz;
    memcpy(buf, &d.data()[fDataPosition - sz], sz);
}

void RawMessage::clear()
{
    fReply.clear();
    //Pattern 0 - 2
    fReply.append(0x03);
    fReply.append(0x04);
    fReply.append(0x15);
    //Message number 3 - 6
    fReply.append('\x00');
    fReply.append('\x00');
    fReply.append('\x00');
    fReply.append('\x00');
    //Message id 7 - 10
    fReply.append('\x00');
    fReply.append('\x00');
    fReply.append('\x00');
    fReply.append('\x00');
    //Message command 11 - 12
    fReply.append('\x00');
    fReply.append('\x00');
    //Data size 13 - 17
    fReply.append('\x00');
    fReply.append('\x00');
    fReply.append('\x00');
    fReply.append('\x00');
    fDataPosition = 0;
}

void RawMessage::clear(const QByteArray &header)
{
    fReply.clear();
    fReply.append(header);;
}

int RawMessage::getPosition()
{
    return fDataPosition;
}

void RawMessage::setPosition(int pos)
{
    fDataPosition = pos;
}

void RawMessage::writeToSocket()
{
    qDebug() << "Data size" << data().size() << "bytes";
    qDebug() << "Socket write" << fSocket->write(data()) << "bytes";
}

quint32 RawMessage::getDataSize()
{
    quint32 sz;
    memcpy(reinterpret_cast<char*>(&sz), &fReply.data()[13], sizeof(quint32));
    return sz;
}

void RawMessage::increaseDataSize(quint32 sz)
{
    quint32 newSize = getDataSize() + sz;
    setDataSize(newSize);
}
