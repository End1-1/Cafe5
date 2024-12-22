#ifndef SQLQUERY_H
#define SQLQUERY_H

#include <QJsonObject>

extern "C" Q_DECL_EXPORT bool newmessage(const QJsonObject &jreq, QJsonObject &jret, QString &err);
extern "C" Q_DECL_EXPORT bool getunread(const QJsonObject &jreq, QJsonObject &jret, QString &err);
extern "C" Q_DECL_EXPORT bool clearbasket(const QJsonObject &jreq, QJsonObject &jret, QString &err);
#endif // SQLQUERY_H
