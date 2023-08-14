#ifndef STRUCTS_H
#define STRUCTS_H

#include <QMetaType>

struct CoordinateData {
    double latitude;
    double longitude;
    double speed;
    double azimuth;
    double datetime;
};

struct ConnectionStatus {
    quint32 user;
    bool online;
    bool authorized;
    CoordinateData lastCoordinate;
};

#endif // STRUCTS_H
