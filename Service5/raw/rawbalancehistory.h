#ifndef RAWBALANCEHISTORY_H
#define RAWBALANCEHISTORY_H

#include "raw.h"

class RawBalanceHistory : public Raw
{
    Q_OBJECT
public:
    RawBalanceHistory(SslSocket *s, const QByteArray &d);

public slots:
    virtual void run() override;
};

#endif // RAWBALANCEHISTORY_H