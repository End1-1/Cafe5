#ifndef JSONUTILS_H
#define JSONUTILS_H

#include "c5database.h"
#include <QJsonArray>

QJsonArray fetchFromDb(C5Database &db);

#endif // JSONUTILS_H
