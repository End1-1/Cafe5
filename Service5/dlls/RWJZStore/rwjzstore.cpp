#include "rwjzstore.h"
#include "rawmessage.h"

bool dllfunc(const QByteArray &in, QByteArray &out)
{
    RawMessage r(nullptr);
    QString dllname;
    quint16 year;
    quint8 month;
    quint8 cafe;
    quint8 store;
    r.readString(dllname, in);
    r.readUShort(year, in);
    r.readUByte(month, in);
    r.readUByte(cafe, in);
    r.readUByte(store, in);
    return true;
}
