#ifndef DLL_H
#define DLL_H

#include "c5database.h"
#include <QObject>
#include <QStringList>
#include <QJsonObject>
#include <QTranslator>

extern "C" Q_DECL_EXPORT QString caption();
extern "C" Q_DECL_EXPORT QJsonArray json(C5Database &db, const QJsonObject &params);
extern "C" Q_DECL_EXPORT void translator(QTranslator &trans);

#endif // DLL_H
