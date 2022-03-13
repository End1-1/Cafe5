#ifndef RAWBALANCEHISTORY_H
#define RAWBALANCEHISTORY_H

#include "raw.h"

class RawBalanceHistory : public Raw
{
    Q_OBJECT
public:
    RawBalanceHistory(SslSocket *s);
    ~RawBalanceHistory();

public slots:
    virtual void run(const QByteArray &d) override;

private:
    void balanceAmountTotal();
};

#endif // RAWBALANCEHISTORY_H
