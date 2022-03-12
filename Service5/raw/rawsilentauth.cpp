#include "rawsilentauth.h"
#include "rawdataexchange.h"

RawSilentAuth::RawSilentAuth(SslSocket *s, const QByteArray &d) :
    Raw(s, d)
{
    connect(this, &RawSilentAuth::auth, RawDataExchange::instance(), &RawDataExchange::silentAuth);
}

RawSilentAuth::~RawSilentAuth()
{
    disconnect(this, &RawSilentAuth::auth, RawDataExchange::instance(), &RawDataExchange::silentAuth);
}

void RawSilentAuth::run()
{
    QString phone = readString();
    QString password = readString();
    emit auth(fSocket, phone, password);
}
