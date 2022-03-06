#include "rawhello.h"
#include "databaseconnectionmanager.h"
#include "logwriter.h"
#include "rawdataexchange.h"

RawHello::RawHello(SslSocket *s, const QByteArray &d) :
    Raw(s, d)
{

}

void RawHello::run()
{
    quint32 strLen;
    memcpy(&strLen, fData.data(), sizeof(strLen));
    QString firebaseToken = fData.mid(4, fData.length() - 4);
    RawDataExchange::instance()->registerFirebaseToken(firebaseToken, fSocket);
    emit finish();
}
