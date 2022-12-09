#include "ops.h"

bool rmerror(RawMessage &rm, const QString &err)
{
    rm.putUByte(0);
    rm.putString(err);
    return false;
}
