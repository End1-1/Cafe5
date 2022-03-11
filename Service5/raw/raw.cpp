#include "raw.h"
#include "rawdataexchange.h"

Raw::Raw(SslSocket *s, const QByteArray &d) :
    RawMessage(s, d)
{
    connect(RawDataExchange::instance(), &RawDataExchange::socketData, this, &Raw::receiveReply);
}

void Raw::receiveReply(SslSocket *s, const QByteArray &d)
{
    Q_UNUSED(d);
    if (s == fSocket) {
        emit finish(d);
    }
}
