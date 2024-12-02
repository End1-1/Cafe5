#ifndef SQLQUERY_H
#define SQLQUERY_H

#include <QJsonObject>

#ifdef MYLIB_EXPORTS
    #define MYLIB_SHARED_EXPORT Q_DECL_EXPORT
#else
    #define MYLIB_SHARED_EXPORT Q_DECL_IMPORT
#endif

extern "C" MYLIB_SHARED_EXPORT bool networkdb();
extern "C" MYLIB_SHARED_EXPORT bool printservice(const QJsonObject &jreq, QJsonObject &jrep, QString &err);
extern "C" MYLIB_SHARED_EXPORT bool printfiscal(const QJsonObject &jreq, QJsonObject &jrep, QString &err);
extern "C" MYLIB_SHARED_EXPORT bool printbill(const QJsonObject &jreq, QJsonObject &jrep, QString &err);

#endif // SQLQUERY_H
