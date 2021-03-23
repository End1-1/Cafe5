#include "socketdata.h"

QByteArray getData(const QByteArray &data, const DataAddress &da)
{
    return data.mid(da.start, da.length);
}

