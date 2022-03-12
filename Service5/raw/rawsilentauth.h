#ifndef RAWSILENTAUTH_H
#define RAWSILENTAUTH_H

#include "raw.h"

class RawSilentAuth : public Raw
{
    Q_OBJECT
public:
    RawSilentAuth(SslSocket *s, const QByteArray &d);
    ~RawSilentAuth();

public slots:
    virtual void run() override;

signals:
    void auth(SslSocket*, const QString&, const QString&);
};

#endif // RAWSILENTAUTH_H
