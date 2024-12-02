#ifndef WAITERSERVICE_H
#define WAITERSERVICE_H

#include "waiterservice_global.h"
#include <QJsonObject>

extern "C" WAITERSERVICE_EXPORT bool printfiscal(const QJsonObject &jrep, QJsonObject &jret, QString &err);

#endif // WAITERSERVICE_H
