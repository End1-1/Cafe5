#ifndef JZSTORE_H
#define JZSTORE_H

#include "socketdata.h"
#include "requesthandler.h"
#include <QStringList>

void init();
bool requestGoodsGroups(RequestHandler &rh, const QByteArray &data, const QHash<QString, DataAddress> &dataMap);
bool requestStore(RequestHandler &rh, const QByteArray &data, const QHash<QString, DataAddress> &dataMap);
void addConnection(int id, const QString &name, const QString &host, const QString &schema, const QString &username, const QString &password);
extern "C" Q_DECL_EXPORT void routes(QStringList &r);
extern "C" Q_DECL_EXPORT bool jzstore(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType);

#endif // JZSTORE_H
