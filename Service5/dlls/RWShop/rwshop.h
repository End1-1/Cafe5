#ifndef RWSHOP_H
#define RWSHOP_H

#include "rawmessage.h"
#include <QByteArray>

extern "C" Q_DECL_EXPORT bool dllfunc(const QByteArray &in, RawMessage &rm);

#endif // RWSHOP_H
