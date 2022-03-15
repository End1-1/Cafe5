#include "rawcarnear.h"
#include <math.h>
#include <cmath>

#define earthRadiusKm 6371.0
#define M_PI 3.14159265

double deg2rad(double deg) {
  return (deg * M_PI / 180);
}

//  This function converts radians to decimal degrees
double rad2deg(double rad) {
  return (rad * 180 / M_PI);
}

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

    QVector<ConnectionStatus> near;
    for (auto &c: fMapTokenConnectionStatus) {
        if (c.online == false) {
            continue;
        }
        if (c.lastCoordinate.datetime < 1) {
            continue;
        }
        double lat1r, lon1r, lat2r, lon2r, u, v;
        lat1r = deg2rad(latitude);
        lon1r = deg2rad(longitude);
        lat2r = deg2rad(c.lastCoordinate.latitude);
        lon2r = deg2rad(c.lastCoordinate.longitude);
        u = sin((lat2r - lat1r)/2);
        v = sin((lon2r - lon1r)/2);
        double d = 2.0 * earthRadiusKm * asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v));
        if (d * 1000 < radius) {
            near.append(c);
        }
    }

    putUInt(near.count());
    for (auto &c: near) {
        putUInt(c.user);
        putDouble(c.lastCoordinate.latitude);
        putDouble(c.lastCoordinate.longitude);
    }
}
