#ifndef RAWHELLO_H
#define RAWHELLO_H

#include "raw.h"

class RawHello : public Raw
{
    Q_OBJECT

public:
    RawHello(SslSocket *s);
    ~RawHello();

public slots:
    virtual int run(const QByteArray &d) override;
};

#endif // RAWHELLO_H
