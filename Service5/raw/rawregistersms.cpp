#include "rawregistersms.h"
#include "rawdataexchange.h"

RawRegisterSMS::RawRegisterSMS(SslSocket *s, const QByteArray &d) :
    Raw(s, d)
{
    connect(this, &RawRegisterSMS::registerSms, RawDataExchange::instance(), &RawDataExchange::registerSms);
}

void RawRegisterSMS::run()
{
    QString sms = readString();
    emit registerSms(fSocket, sms);
}
