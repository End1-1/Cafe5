#include "rawbalancehistory.h"
#include "rawdataexchange.h"

RawBalanceHistory::RawBalanceHistory(SslSocket *s, const QByteArray &d) :
    Raw(s, d)
{
    connect(this, &RawBalanceHistory::balanceAmountTotal, RawDataExchange::instance(), &RawDataExchange::balanceAmountTotal);
}

void RawBalanceHistory::run()
{
    quint8 h = readUByte();
    switch (h) {
    case 1:
        emit balanceAmountTotal(fSocket);
        break;
    case 2:
        break;
    }
}
