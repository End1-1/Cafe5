#ifndef WAITERCLIENT_H
#define WAITERCLIENT_H

#include "rawmessage.h"
#include <QByteArray>

extern "C" Q_DECL_EXPORT bool dllfunc(const QByteArray &in, RawMessage &rm);
#endif // WAITERCLIENT_H
