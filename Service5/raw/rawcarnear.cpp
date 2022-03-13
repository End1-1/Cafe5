#include "rawcarnear.h"

RawCarNear::RawCarNear(SslSocket *s) :
    Raw(s)
{

}

void RawCarNear::run(const QByteArray &d)
{
    quint32 radius;
    double latitude, longitude;
    readUInt(radius, d);
    readDouble(latitude, d);
    readDouble(longitude, d);
}
