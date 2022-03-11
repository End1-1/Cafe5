#include "rawmessage.h"

RawMessage::RawMessage(SslSocket *s, const QByteArray &d) :
    QObject(nullptr),
    fSocket(s),
    fData(d)
{
    clear();
}

RawMessage::~RawMessage()
{
    qDebug() << metaObject()->className();
}

void RawMessage::setHeader(quint32 msgNum, quint32 msgId, quint16 msgType)
{
    setHeader(fReply, msgNum, msgId, msgType);
}

void RawMessage::setHeader(QByteArray &d, quint32 msgNum, quint32 msgId, quint16 msgType)
{
    d.replace(3, 4, reinterpret_cast<const char*>(&msgNum), sizeof(msgNum));
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

void RawMessage::putUInt(quint32 v)
{
    fReply.append(reinterpret_cast<const char*>(&v), sizeof(quint32));
    increaseDataSize(sizeof(quint32));
}

void RawMessage::putDouble(double v)
{
    fReply.append(reinterpret_cast<const char*>(&v), sizeof(double));
    increaseDataSize(sizeof(double));
}

void RawMessage::putString(const QString &v)
{
    quint32 sz = v.toUtf8().length();
    putUInt(sz);
    fReply.append(v.toUtf8());
    increaseDataSize(sz);
}

void RawMessage::putBytes(const char *data, quint32 size)
{
    putUInt(size);
    fReply.append(data, size);
    increaseDataSize(size);
}

quint8 RawMessage::readUByte()
{
    quint8 i;
    memcpy(reinterpret_cast<char*>(&i), &fData.data()[fDataPosition], sizeof(quint8));
    fDataPosition += sizeof(quint8);
    return i;
}

quint32 RawMessage::readUInt()
{
    quint32 i;
    memcpy(reinterpret_cast<char*>(&i), &fData.data()[fDataPosition], sizeof(quint32));
    fDataPosition += sizeof(quint32);
    return i;
}

const QString RawMessage::readString()
{
    quint32 sz = readUInt();
    fDataPosition += sz;
    return fData.mid(fDataPosition - sz, sz);
}

void RawMessage::readBytes(char *buf)
{
    quint32 sz = readUInt();
    fDataPosition += sz;
    memcpy(buf, &fData.data()[fDataPosition - sz], sz);
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
