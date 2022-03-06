#ifndef RAWCOORDINATE_H
#define RAWCOORDINATE_H

#include "raw.h"
class RawCoordinate : public Raw
{
    Q_OBJECT

    struct CoordinateData {
        double latitude;
        double longitude;
        double speed;
        double azimuth;
        double datetime;
    };

public:
    explicit RawCoordinate(SslSocket *s, const QByteArray &d);

public slots:
    virtual void run() override;
};

#endif // RAWCOORDINATE_H
