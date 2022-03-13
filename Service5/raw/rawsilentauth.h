#ifndef RAWSILENTAUTH_H
#define RAWSILENTAUTH_H

#include "raw.h"

class RawSilentAuth : public Raw
{
    Q_OBJECT
public:
    RawSilentAuth(SslSocket *s);
    ~RawSilentAuth();

public slots:
    virtual void run(const QByteArray &d) override;
};

#endif // RAWSILENTAUTH_H
