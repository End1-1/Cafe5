#include "raw.h"
#include <QDebug>

Raw::Raw(SslSocket *s, const QByteArray &d) :
    QObject(nullptr),
    fData(d),
    fSocket(s)
{
    //Pattern
    fReply.append(0x03);
    fReply.append(0x04);
    fReply.append(0x15);
    //Message number
    fReply.append('\x00');
    fReply.append('\x00');
    fReply.append('\x00');
    fReply.append('\x00');
    //Message id
    fReply.append('\x00');
    fReply.append('\x00');
    fReply.append('\x00');
    fReply.append('\x00');
    //Message command
    fReply.append('\x00');
    fReply.append('\x00');
    //Data size
    fReply.append('\x00');
    fReply.append('\x00');
    fReply.append('\x00');
    fReply.append('\x00');
    fDataPosition = 0;
}

void Raw::setHeader(quint32 msgNum, quint32 msgId, quint16 msgType)
{
    fReply.replace(3, 4, reinterpret_cast<const char*>(&msgNum), sizeof(msgNum));
    fReply.replace(7, 4, reinterpret_cast<const char*>(&msgId), sizeof(msgId));
    fReply.replace(11, 2, reinterpret_cast<const char*>(&msgType), sizeof(msgType));
}

const QByteArray &Raw::data()
{
    return fReply;
}

void Raw::putUByte(quint8 v)
{
    fReply.append(reinterpret_cast<const char*>(&v), sizeof(quint8));
    increaseDataSize(sizeof(quint8));
}

void Raw::setDataSize(quint32 sz)
{
    fReply.replace(13, 4, reinterpret_cast<const char*>(&sz), sizeof(sz));
}

void Raw::putUShort(quint16 v)
{
    fReply.append(reinterpret_cast<const char*>(&v), sizeof(quint16));
    increaseDataSize(sizeof(quint16));
}

void Raw::putUInt(quint32 v)
{
    fReply.append(reinterpret_cast<const char*>(&v), sizeof(quint32));
    increaseDataSize(sizeof(quint32));
}

void Raw::putDouble(double v)
{
    fReply.append(reinterpret_cast<const char*>(&v), sizeof(double));
    increaseDataSize(sizeof(double));
}

void Raw::putString(const QString &v)
{
    quint32 sz = v.toUtf8().length();
    putUInt(sz);
    fReply.append(v.toUtf8());
    increaseDataSize(sz);
}

quint8 Raw::readUByte()
{
    quint8 i;
    memcpy(reinterpret_cast<char*>(&i), &fData.data()[fDataPosition], sizeof(quint8));
    fDataPosition += sizeof(quint8);
    return i;
}

quint32 Raw::readUInt()
{
    quint32 i;
    memcpy(reinterpret_cast<char*>(&i), &fData.data()[fDataPosition], sizeof(quint32));
    fDataPosition += sizeof(quint32);
    return i;
}

const QString Raw::readString()
{
    quint32 sz = readUInt();
    fDataPosition += sz;
    return fData.mid(fDataPosition - sz, sz);
}

quint32 Raw::getDataSize()
{
    quint32 sz;
    memcpy(reinterpret_cast<char*>(&sz), &fReply.data()[13], sizeof(quint32));
    return sz;
}

void Raw::increaseDataSize(quint32 sz)
{
    quint32 newSize = getDataSize() + sz;
    setDataSize(newSize);
}
