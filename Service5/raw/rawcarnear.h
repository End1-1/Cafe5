#ifndef RAWCARNEAR_H
#define RAWCARNEAR_H

#include "raw.h"

class RawCarNear : public Raw
{
public:
    RawCarNear(SslSocket *s);

public slots:
    virtual int run(const QByteArray &d) override;
};

#endif // RAWCARNEAR_H
