#ifndef SQLQUERY_H
#define SQLQUERY_H

#include <QStringList>

extern "C" Q_DECL_EXPORT void routes(QStringList &r);
extern "C" Q_DECL_EXPORT bool Cafe4Inventory(const QByteArray &indata, QByteArray &outdata,
        const QHash<QString, DataAddress> &dataMap, const ContentType &contentType);

#endif // SQLQUERY_H
