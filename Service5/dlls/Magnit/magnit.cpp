#include "logwriter.h"
#include "commandline.h"
#include "database.h"
#include "armsoft.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

bool armsoft(const QJsonObject &jreq, QJsonObject &jret, QString &err)
{
    CommandLine cl;
    QString path;
    cl.value("dllpath", path);
    Database db;
    QString configFile = path + "/magnit.ini";
    ArmSoft as(jreq["params"].toObject());
    jret["idontknowe"] = as.exportToAS(err);;
    return true;
}
