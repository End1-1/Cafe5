#include "rawcoordinate.h"
#include <QDebug>
#include <QDateTime>

RawCoordinate::RawCoordinate(SslSocket *s, const QByteArray &d) :
    Raw(s, d)
{

}

void RawCoordinate::run()
{
    CoordinateData c;
    memcpy(&c, fData.data(), sizeof(c));
    QDateTime dt;
    dt.setMSecsSinceEpoch(c.datetime);
    qDebug() << c.latitude << c.longitude << c.speed << c.azimuth << c.datetime << dt;
    emit finish();
}
