#ifndef RAWHELLO_H
#define RAWHELLO_H

#include "raw.h"

class RawHello : public Raw
{
public:
    RawHello(SslSocket *s, const QByteArray &d);

public slots:
    virtual void run();
};

#endif // RAWHELLO_H
