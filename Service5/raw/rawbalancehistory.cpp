#include "rawbalancehistory.h"
#include "rawdataexchange.h"

RawBalanceHistory::RawBalanceHistory(SslSocket *s, const QByteArray &d) :
    Raw(s, d)
{

}

void RawBalanceHistory::run()
{
    quint8 h = readUByte();
    quint8 reply = 0;
    switch (h) {
    case 1:
        double amount;
        if (RawDataExchange::instance()->balanceAmount(fSocket, amount)) {
            reply = 1;
            putUByte(reply);
            putDouble(amount);
            emit finish();
            return;
        }
        break;
    case 2:
        break;
    }
}
