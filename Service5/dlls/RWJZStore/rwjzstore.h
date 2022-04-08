#ifndef RWJZSTORE_H
#define RWJZSTORE_H

#include "rawmessage.h"
#include <QByteArray>

extern "C" Q_DECL_EXPORT bool dllfunc(const QByteArray &in, RawMessage &rm);

#endif // RWJZSTORE_H
