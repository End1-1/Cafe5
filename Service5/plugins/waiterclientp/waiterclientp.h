#ifndef WAITERCLIENTP_H
#define WAITERCLIENTP_H

#include "rawmessage.h"
#include "rawpluginobject.h"

extern "C" Q_DECL_EXPORT bool startPlugin(const QString &configFileName);
extern "C" Q_DECL_EXPORT QString pluginUuid();
extern "C" Q_DECL_EXPORT int handler(RawMessage *rm, const QByteArray &in);
extern "C" Q_DECL_EXPORT int removeSocket(SslSocket *s);

#endif // WAITERCLIENTP_H
