#ifndef RWMFTASKS_H
#define RWMFTASKS_H

#include "rawmessage.h"
#include <QByteArray>

extern "C" Q_DECL_EXPORT bool dllfunc(const QByteArray &in, RawMessage &rm);

#endif // RWMFTASKS_H
