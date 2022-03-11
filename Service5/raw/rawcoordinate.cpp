#include "rawcoordinate.h"
#include "rawdataexchange.h"
#include <QDebug>
#include <QDateTime>

#include <QFile>

RawCoordinate::RawCoordinate(SslSocket *s, const QByteArray &d) :
    Raw(s, d)
{
    connect(this, &RawCoordinate::devicePosition, RawDataExchange::instance(), &RawDataExchange::devicePosition);
}

RawCoordinate::~RawCoordinate()
{
    qDebug() << "~RawCoordinate";
}

void RawCoordinate::run()
{
    CoordinateData c;
    memcpy(&c, fData.data(), sizeof(c));
    QFile f("c:\\intel\\charbax-zeytun.dat");
    f.open(QIODevice::Append);
    f.write(reinterpret_cast<const char*>(&c), sizeof(c));
    f.close();
    QDateTime dt;
    dt.setMSecsSinceEpoch(c.datetime);
    emit devicePosition(fSocket, c);
    //qDebug() << c.latitude << c.longitude << c.speed << c.azimuth << c.datetime << dt;
}
