#include "rawregisterphone.h"
#include "rawdataexchange.h"
#include "firebase.h"

RawRegisterPhone::RawRegisterPhone(SslSocket *s, const QByteArray &d) :
    Raw(s, d)
{

}

void RawRegisterPhone::run()
{
    QString phone = readString();
    QString smsCode;
    RawDataExchange::instance()->createRegisterPhoneRequest(phone, smsCode, fSocket);
    putUShort(0);
    auto *f = new Firebase();
    connect(f, &Firebase::sendFinished, this, &RawRegisterPhone::fcmFinished);
    f->sendMessage(RawDataExchange::instance()->tokenOfSocket(fSocket), QString("Your confirmation code: %1").arg(smsCode));
}

void RawRegisterPhone::fcmFinished()
{
    emit finish();
}
