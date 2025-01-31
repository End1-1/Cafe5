#include "armsoft.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

bool armsoft(const QJsonObject &jreq, QJsonObject &jret, QString &err)
{
    ArmSoft as(jreq["params"].toObject());
    jret["idontknowe"] = as.exportToAS(err);;
    return true;
}
