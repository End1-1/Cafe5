#ifndef STANDARD_H
#define STANDARD_H

#include "socketdata.h"
#include <QStringList>

extern "C" Q_DECL_EXPORT void routes(QStringList &r);
extern "C" Q_DECL_EXPORT bool notfound(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType);
extern "C" Q_DECL_EXPORT bool registration(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType);
extern "C" Q_DECL_EXPORT bool confirmregistration(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType);
extern "C" Q_DECL_EXPORT bool authorization(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType);
extern "C" Q_DECL_EXPORT bool chat(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType);

#endif // STANDARD_H
