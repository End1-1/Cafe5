#ifndef RAWCOORDINATE_H
#define RAWCOORDINATE_H

#include "raw.h"
#include "structs.h"

class RawCoordinate : public Raw
{
    Q_OBJECT

public:
    explicit RawCoordinate(SslSocket *s);
    ~RawCoordinate();

public slots:
    virtual void run(const QByteArray &d) override;

};

#endif // RAWCOORDINATE_H
