#include "rawyandexkey.h"

RawYandexKey::RawYandexKey(SslSocket *s, const QByteArray &d) :
    Raw(s, d)
{

}

void RawYandexKey::run()
{
    QString key = "06495363-2976-4cbb-a0b7-f09387554b9d";
    putString(key);
    emit reply(fReply);
}
