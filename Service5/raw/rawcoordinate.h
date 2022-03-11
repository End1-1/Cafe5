#ifndef RAWCOORDINATE_H
#define RAWCOORDINATE_H

#include "raw.h"
#include "structs.h"

class RawCoordinate : public Raw
{
    Q_OBJECT

public:
    explicit RawCoordinate(SslSocket *s, const QByteArray &d);
    ~RawCoordinate();

public slots:
    virtual void run() override;

signals:
    void devicePosition(SslSocket*, const CoordinateData &);
};

#endif // RAWCOORDINATE_H
