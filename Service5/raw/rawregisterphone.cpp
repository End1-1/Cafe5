#include "rawregisterphone.h"
#include "rawdataexchange.h"
#include "firebase.h"

RawRegisterPhone::RawRegisterPhone(SslSocket *s, const QByteArray &d) :
    Raw(s, d)
{
    connect(this, &RawRegisterPhone::registerPhone, RawDataExchange::instance(), &RawDataExchange::registerPhone);
}

void RawRegisterPhone::run()
{
    QString phone = readString();
    emit registerPhone(fSocket, phone);


}
