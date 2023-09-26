#include "sqlquery.h"
#include "jsonhandler.h"
#include "logwriter.h"
#include "requesthandler.h"
#include "commandline.h"
#include "database.h"
#include "queryjsonresponse.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QSettings>

void routes(QStringList &r)
{
    r.append("Cafe4Inventory");
}

bool Cafe4Inventory(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType)
{
    CommandLine cl;
    QString path;
    cl.value("dllpath", path);
    JsonHandler jh;
    RequestHandler rh(outdata);
    QString configFile = path + "/Cafe4Inventory.ini";
    if (!QFile::exists(configFile)) {
        LogWriter::write(LogWriterLevel::errors, "", QString("Cafe4Inventory config file not exists: %1").arg(configFile));
        jh["message"] = QString("Cafe4Inventory config file not exists: %1").arg(configFile);
        return rh.setInternalServerError(jh.toString());
    }
    QSettings s(configFile, QSettings::IniFormat);
    s.setIniCodec("UTF-8");

    if (contentType != ContentType::ApplilcationJson) {
        return rh.setDataValidationError("Accept content type: Applilcation/Json");
    }


    QJsonObject jo = QJsonDocument::fromJson(getData(indata, dataMap["json"])).object();
    QJsonObject jresponse;
    QueryJsonResponse(s, jo, jresponse).getResponse();

    return rh.setResponse(HTTP_OK, QJsonDocument(jresponse).toJson(QJsonDocument::Compact));
}
