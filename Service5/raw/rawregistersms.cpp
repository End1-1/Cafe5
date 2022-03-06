#include "rawregistersms.h"
#include "rawdataexchange.h"

RawRegisterSMS::RawRegisterSMS(SslSocket *s, const QByteArray &d) :
    Raw(s, d)
{

}

void RawRegisterSMS::run()
{
    QString sms = readString();
    short reply = RawDataExchange::instance()->checkPhoneSMSRequest(sms, fSocket) ? 1 : 0;
    putUShort(reply);
    emit finish();
}
