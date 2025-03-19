#ifndef SQLQUERY_H
#define SQLQUERY_H

#include <QJsonObject>

extern "C" Q_DECL_EXPORT bool inventory(const QJsonObject &jreq, QJsonObject &jret, QString &err);

#endif // SQLQUERY_H
