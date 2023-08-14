#ifndef RAWYANDEXKEY_H
#define RAWYANDEXKEY_H

#include "raw.h"

class RawYandexKey : public Raw
{
    Q_OBJECT
public:
    RawYandexKey(SslSocket *s);

public slots:
    virtual int run(const QByteArray &d) override;
};

#endif // RAWYANDEXKEY_H
