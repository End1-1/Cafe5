#include "rawcoordinate.h"
#include "messagelist.h"
#include <QDebug>
#include <QDateTime>
#include <QFile>

RawCoordinate::RawCoordinate(SslSocket *s) :
    Raw(s)
{

}

RawCoordinate::~RawCoordinate()
{
    qDebug() << "~RawCoordinate";
}

int RawCoordinate::run(const QByteArray &d)
{
    CoordinateData c;
    memcpy(&c, d.data(), sizeof(c));
//    QFile f("c:\\intel\\charbax-zeytun.dat");
//    f.open(QIODevice::Append);
//    f.write(reinterpret_cast<const char*>(&c), sizeof(c));
//    f.close();
    QDateTime dt;
    dt.setMSecsSinceEpoch(c.datetime);

    QString token = fMapSocketToken[fSocket];
    int user = fMapTokenUser[token];
    fMapTokenConnectionStatus[token].lastCoordinate = c;
    setHeader(0, 0, MessageList::srv_device_position);
    putUInt(user);
    putBytes(reinterpret_cast<const char*>(&c), sizeof(c));
    informMonitors(fReply);
    return 0;
}
