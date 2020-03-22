#include "socketdata.h"

QString SocketData::getValue(int key) const
{
    return fSettings[key];
}
