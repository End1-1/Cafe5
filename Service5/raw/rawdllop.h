#ifndef RAWDLLOP_H
#define RAWDLLOP_H

#include "raw.h"

class RawDllOp: public Raw
{
public:
    RawDllOp(SslSocket *s);
    ~RawDllOp();

public slots:
    virtual int run(const QByteArray &d) override;
};

#endif // RAWDLLOP_H
