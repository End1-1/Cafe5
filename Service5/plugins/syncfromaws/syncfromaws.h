#ifndef SYNCFROMAWS_H
#define SYNCFROMAWS_H

#include "rawmessage.h"

extern "C" Q_DECL_EXPORT bool startPlugin(const QString &configFileName);
extern "C" Q_DECL_EXPORT QString pluginUuid();
extern "C" Q_DECL_EXPORT int handler(RawMessage *rm, const QByteArray &in);
extern "C" Q_DECL_EXPORT int removeSocket(SslSocket *s);

#endif // SYNCFROMAWS_H
