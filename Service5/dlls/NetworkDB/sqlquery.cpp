#include "sqlquery.h"
#include "jsonhandler.h"
#include "logwriter.h"
#include "requesthandler.h"
#include "commandline.h"
#include "queryjsonresponse.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QFile>

void routes(QStringList &r)
{
    r.append("networkdb");
    r.append("printservice");
}

bool networkdb(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType)
{
    CommandLine cl;
    QString path;
    cl.value("dllpath", path);
    JsonHandler jh;
    RequestHandler rh(outdata);

    if (contentType != ContentType::ApplilcationJson) {
        return rh.setDataValidationError("Accept content type: Applilcation/Json");
    }
    QString configFile = path + "/networkdb.ini";
    if (!QFile::exists(configFile)) {
        LogWriter::write(LogWriterLevel::errors, "", QString("sqlquery config path not exists: %1").arg(configFile));
        jh["message"] = "Server not configured";
        return rh.setInternalServerError(jh.toString());
    }
    QSettings s(configFile, QSettings::IniFormat);
    QJsonObject jo = QJsonDocument::fromJson(getData(indata, dataMap["json"])).object();
    QJsonObject jresponse;
    QueryJsonResponse(s.value("host/host").toString(), jo, jresponse).getResponse();

    return rh.setResponse(HTTP_OK, QJsonDocument(jresponse).toJson(QJsonDocument::Compact));
}

bool printservice(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType)
{
    CommandLine cl;
    QString path;
    cl.value("dllpath", path);
    JsonHandler jh;
    RequestHandler rh(outdata);

    if (contentType != ContentType::ApplilcationJson) {
        return rh.setDataValidationError("Accept content type: Applilcation/Json");
    }
    QString configFile = path + "/networkdb.ini";
    if (!QFile::exists(configFile)) {
        LogWriter::write(LogWriterLevel::errors, "", QString("sqlquery config path not exists: %1").arg(configFile));
        jh["message"] = "Server not configured";
        return rh.setInternalServerError(jh.toString());
    }
    QSettings s(configFile, QSettings::IniFormat);
    QJsonObject jo = QJsonDocument::fromJson(getData(indata, dataMap["json"])).object();
    QJsonObject jresponse;

    QString sql = "select ";

    return rh.setResponse(HTTP_OK, QJsonDocument(jresponse).toJson(QJsonDocument::Compact));
}
