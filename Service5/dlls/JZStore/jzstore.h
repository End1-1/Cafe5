#ifndef JZSTORE_H
#define JZSTORE_H

#include <QStringList>
#include <QJsonObject>

void init();
bool requestGoodsGroups(const QJsonObject &jreq, QJsonObject &jret, QString &err);
bool requestStore(const QJsonObject &jreq, QJsonObject &jret, QString &err);
void addConnection(int id, const QString &name, const QString &host, const QString &schema, const QString &username,
                   const QString &password);
extern "C" Q_DECL_EXPORT bool jzstore(const QJsonObject &jreq, QJsonObject &jret, QString &err);

#endif // JZSTORE_H
