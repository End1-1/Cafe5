#include "sqlquery.h"
#include "jsonhandler.h"
#include "logwriter.h"
#include "requesthandler.h"
#include "commandline.h"
#include "printtaxn.h"
#include "database.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

void routes(QStringList &r)
{
    r.append("fiscal");
}

bool fiscal(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap,
            const ContentType &contentType)
{
    CommandLine cl;
    QString path;
    cl.value("dllpath", path);
    Database db;
    JsonHandler jh;
    RequestHandler rh(outdata);
    if (contentType != ContentType::ApplilcationJson) {
        return rh.setDataValidationError("Accept content type: Application/Json");
    }
    qDebug() << indata;
    QJsonObject jo = QJsonDocument::fromJson(getData(indata, dataMap["json"])).object();
    qDebug() << jo["key"].toString();
    if (jo["key"].toString() != "asd666649888d!!jjdjmskkak98983mj???m") {
        return rh.setResponse(HTTP_FORBIDDEN, "Access denied");
    }
    QString err;
    QByteArray body = QJsonDocument(jo["hdm"].toObject()).toJson(QJsonDocument::Compact);
    PrintTaxN pt(jo["ip"].toString(),
                 jo["port"].toInt(),
                 jo["password"].toString(),
                 jo["extpos"].toString(),
                 jo["opcode"].toString(),
                 jo["oppin"].toString());
    int res = pt.printJSON(body, err, (quint8) jo["action"].toInt());
#ifdef QT_DEBUG
    QJsonObject jr ;
    jr["response"] = jo["debug_res"].toInt() == 0 ?  res : jo["debug_res"].toInt();
    jr["error"] = err;
    jr["data"] = QJsonDocument::fromJson(
                     QString("{\"rseq\":77,\"crn\":\"63219817\",\"sn\":\"V98745506068\",\"tin\":\"01588771\",\"taxpayer\":\"«Ռոգա էնդ կոպիտա ՍՊԸ»\",\"address\":\"Արշակունյանց 34\",\"time\":1676794194840,\"fiscal\":\"98198105\",\"lottery\":\"00000000\",\"prize\":0,\"total\":1540.0,\"change\":0.0}").toUtf8()).object();
#else
    QJsonObject jr ;
    jr["response"] = res;
    jr["error"] = err;
    jr["data"] = QJsonDocument::fromJson(body).object();
#endif
    return rh.setResponse(HTTP_OK, QJsonDocument(jr).toJson(QJsonDocument::Compact));
}
