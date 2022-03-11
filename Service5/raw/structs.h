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

Q_DECLARE_METATYPE(CoordinateData)

#endif // STRUCTS_H
