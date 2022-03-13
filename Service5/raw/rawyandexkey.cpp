#include "rawyandexkey.h"

RawYandexKey::RawYandexKey(SslSocket *s) :
    Raw(s)
{

}

void RawYandexKey::run(const QByteArray &d)
{
    QString key = "06495363-2976-4cbb-a0b7-f09387554b9d";
    putString(key);
}
