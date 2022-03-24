#ifndef ELINASHOP_H
#define ELINASHOP_H

#include "socketdata.h"
#include "requesthandler.h"
#include "database.h"
#include <QStringList>
#include <QJsonObject>

bool writeOutput(const QString &docId, QString &err, Database &db);
bool writeMovement(const QString &orderid, int goods, double qty, int srcStore, Database &db);
void checkQty(QJsonObject &ord, const QString &orderid, Database &db);
bool validateMultipartFormData(RequestHandler &rh, QJsonObject &ord, const QByteArray &data, const QHash<QString, DataAddress> &dataMap);
inline bool validateApplicationJson(RequestHandler &rh, QJsonObject &ord, const QByteArray &data, const QHash<QString, DataAddress> &dataMap);
extern "C" Q_DECL_EXPORT void routes(QStringList &r);
extern "C" Q_DECL_EXPORT bool shoprequest(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType);
extern "C" Q_DECL_EXPORT bool storerequest(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType);
extern "C" Q_DECL_EXPORT bool printtax(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType);

#endif // ELINASHOP_H
