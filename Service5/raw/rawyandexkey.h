#ifndef RAWYANDEXKEY_H
#define RAWYANDEXKEY_H

#include "raw.h"

class RawYandexKey : public Raw
{
    Q_OBJECT
public:
    RawYandexKey(SslSocket *s, const QByteArray &d);

public slots:
    virtual void run() override;
};

#endif // RAWYANDEXKEY_H
