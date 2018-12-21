#ifndef DLL_H
#define DLL_H

#include "c5database.h"
#include <QObject>
#include <QStringList>
#include <QJsonObject>

extern "C" Q_DECL_EXPORT QString caption();
extern "C" Q_DECL_EXPORT QJsonArray json(C5Database &db, const QJsonObject &params);

#endif // DLL_H
