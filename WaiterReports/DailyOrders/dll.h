#ifndef DLL_H
#define DLL_H

#include "c5database.h"
#include <QObject>
#include <QStringList>
#include <QJsonObject>
#include <QTranslator>

extern "C" Q_DECL_EXPORT void caption(QString &str);
extern "C" Q_DECL_EXPORT void json(C5Database &db, const QJsonObject &params, QJsonArray &jarr);
extern "C" Q_DECL_EXPORT void translator(QTranslator &trans);

#endif // DLL_H
