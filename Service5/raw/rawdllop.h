#ifndef RAWDLLOP_H
#define RAWDLLOP_H

#include "raw.h"

class RawDllOp: public Raw
{
public:
    RawDllOp(SslSocket *s);

public slots:
    virtual void run(const QByteArray &d) override;
};

#endif // RAWDLLOP_H
