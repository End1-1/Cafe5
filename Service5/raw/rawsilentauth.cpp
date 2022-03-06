#include "rawsilentauth.h"
#include "rawdataexchange.h"

RawSilentAuth::RawSilentAuth(SslSocket *s, const QByteArray &d) :
    Raw(s, d)
{

}

void RawSilentAuth::run()
{
    QString phone = readString();
    QString password = readString();
    quint8 reply = RawDataExchange::instance()->silentAuth(phone, password) ? 1 : 0;
    putUByte(reply);
    emit finish();
}
