#ifndef RWJZ_H
#define RWJZ_H

#include "rawmessage.h"
#include <QByteArray>

extern "C" Q_DECL_EXPORT bool dllfunc(const QByteArray &in, RawMessage &rm);

#endif // RWJZ_H
