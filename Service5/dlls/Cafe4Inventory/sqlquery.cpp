
#include "logwriter.h"
#include "queryjsonresponse.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QSettings>
#include <QDir>

extern "C" Q_DECL_EXPORT bool inventory(const QJsonObject &jreq, QJsonObject &jret, QString &err)
{
    QDir d;
    QString dr = d.homePath() + "/Cafe4Inventory";
    if (!d.exists(dr)) {
        d.mkpath(dr);
    }
    QString configFile = dr + "/Cafe4Inventory.ini";
    if (!QFile::exists(configFile)) {
        LogWriter::write(LogWriterLevel::errors, "", QString("Cafe4Inventory config file not exists: %1").arg(configFile));
        jret["errorCode"] = 1;
        jret["errorMessage"] = QString("Cafe4Inventory config file not exists: %1").arg(configFile);
        return false;
    }
    QSettings s(configFile, QSettings::IniFormat);
    QueryJsonResponse(s, jreq, jret).getResponse();
    return true;
}
