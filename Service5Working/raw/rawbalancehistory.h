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
    virtual int run(const QByteArray &d) override;

private:
    void balanceAmountTotal();
    void balanceDetails(const QDateTime &d1, const QDateTime &d2);
};

#endif // RAWBALANCEHISTORY_H
