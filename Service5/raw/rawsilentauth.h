#ifndef RAWSILENTAUTH_H
#define RAWSILENTAUTH_H

#include "raw.h"

class RawSilentAuth : public Raw
{
    Q_OBJECT
public:
    RawSilentAuth(SslSocket *s, const QByteArray &d);

public slots:
    virtual void run() override;
};

#endif // RAWSILENTAUTH_H