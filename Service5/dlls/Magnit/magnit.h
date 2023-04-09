#ifndef MAGNIT_H
#define MAGNIT_H

#include "socketdata.h"
#include <QStringList>

extern "C" Q_DECL_EXPORT void routes(QStringList &r);
extern "C" Q_DECL_EXPORT bool magnit(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType);

#endif // MAGNIT_H
