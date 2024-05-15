#include "sqlquery.h"
#include "jsonhandler.h"
#include "logwriter.h"
#include "requesthandler.h"
#include "commandline.h"
#include "database.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

void routes(QStringList &r)
{
    r.append("office");
    r.append("shop");
}

bool office(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType)
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
    if (jo["key"].toString() != "asdf7fa8kk49888d!!jjdjmskkak98983mj???m") {
        return rh.setResponse(HTTP_FORBIDDEN, "Access denied");
    }

QJsonArray ja;
#ifdef REMOTE_VALSH
    jo = QJsonObject();
    jo["name"] = "Archive";
    jo["waiter_server"] = "";
    jo["host"] = "valsh.picassocloud.com/info.php";
    jo["database"] = "https://valsh.picassocloud.com/";
    jo["username"] = "";
    jo["password"] = "";
    jo["settings"] = "Main";
    jo["fullscreen"] = "";
    jo["local"] = 0;
    ja.append(jo);
#ifdef QT_DEBUG
    jo = QJsonObject();
    jo["name"] = "ValShin";
    jo["waiter_server"] = "";
    jo["host"] = "127.0.0.1/engine/info.php";
    jo["database"] = "https://127.0.0.1/";
    jo["username"] = "";
    jo["password"] = "";
    jo["settings"] = "Main";
    jo["fullscreen"] = "";
    jo["local"] = 1;
    ja.append(jo);
#else
    jo = QJsonObject();
    jo["name"] = "ValShin";
    jo["waiter_server"] = "";
    jo["host"] = "192.168.88.5/engine/info.php";
    jo["database"] = "https://192.168.88.5/";
    jo["username"] = "";
    jo["password"] = "";
    jo["settings"] = "Main";
    jo["fullscreen"] = "";
    jo["local"] = 1;
    ja.append(jo);
#endif
#endif

#ifdef REMOTE_ELINA
    jo = QJsonObject();
    jo["name"] = "Archive";
    jo["waiter_server"] = "";
    jo["host"] = "aws.elina.am/info.php";
    jo["database"] = "https://aws.elina.am/";
    jo["username"] = "";
    jo["password"] = "";
    jo["settings"] = "Main";
    jo["fullscreen"] = "";
    jo["local"] = 0;
    ja.append(jo);
#ifdef QT_DEBUG
    jo = QJsonObject();
    jo["name"] = "Elina";
    jo["waiter_server"] = "";
    jo["host"] = "127.0.0.1/engine/info.php";
    jo["database"] = "https://127.0.0.1/";
    jo["username"] = "";
    jo["password"] = "";
    jo["settings"] = "Main";
    jo["fullscreen"] = "";
    jo["local"] = 1;
    ja.append(jo);
#else
    jo = QJsonObject();
    jo["name"] = "Elina";
    jo["waiter_server"] = "";
    jo["host"] = "192.168.88.42/engine/info.php";
    jo["database"] = "https://192.168.88.42/";
    jo["username"] = "";
    jo["password"] = "";
    jo["settings"] = "Main";
    jo["fullscreen"] = "";
    jo["local"] = 1;
    ja.append(jo);
#endif
#endif

#ifdef REMOTE_DEBUG
    jo = QJsonObject();
    jo["name"] = "Valsh";
    jo["waiter_server"] = "";
    jo["host"] = "valsh.picassocloud.com/info.php";
    jo["database"] = "https://valsh.picassocloud.com/";
    jo["username"] = "";
    jo["password"] = "";
    jo["settings"] = "Main";
    jo["fullscreen"] = "";
    jo["local"] = 0;
    ja.append(jo);
    jo = QJsonObject();
    jo["name"] = "Elina";
    jo["waiter_server"] = "";
    jo["host"] = "aws.elina.am/info.php";
    jo["database"] = "https://aws.elina.am/";
    jo["username"] = "";
    jo["password"] = "";
    jo["settings"] = "Main";
    jo["fullscreen"] = "";
    jo["local"] = 0;
    ja.append(jo);
    jo = QJsonObject();
    jo["name"] = "Carwash";
    jo["waiter_server"] = "";
    jo["host"] = "carwash.picassocloud.com/engine/info.php";
    jo["database"] = "https://carwash.picassocloud.com/";
    jo["username"] = "";
    jo["password"] = "";
    jo["settings"] = "Main";
    jo["fullscreen"] = "";
    jo["local"] = 0;
    ja.append(jo);
#endif

#ifdef REMOTE_ALL
    QString configFile = path + "/servername.ini";
    if (!QFile::exists(configFile)) {
        LogWriter::write(LogWriterLevel::errors, "", QString("sqlquery config path not exists: %1").arg(configFile));
        jh["message"] = "Server not configured";
        return rh.setInternalServerError(jh.toString());
    }
    QFile f(configFile);
    f.open(QIODevice::ReadOnly);
    QJsonObject jconf = QJsonDocument::fromJson(f.readAll()).object();
    f.close();
    ja.append(jconf[jo["params"].toObject()["name"].toString()].toObject());
#endif

    return rh.setResponse(HTTP_OK, QJsonDocument(ja).toJson(QJsonDocument::Compact));
}

bool shop(const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, const ContentType &contentType)
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
    if (jo["key"].toString() != "asdf7fa8kk49888d!!jjdjmskkak98983mj???m") {
        return rh.setResponse(HTTP_FORBIDDEN, "Access denied");
    }


QJsonArray ja;
#ifdef REMOTE_VALSH
#ifdef QT_DEBUG
    jo = QJsonObject();
    jo["name"] = "ValShin";
    jo["waiter_server"] = "";
    jo["host"] = "127.0.0.1/engine/info.php";
    jo["database"] = "https://127.0.0.1/";
    jo["username"] = "";
    jo["password"] = "";
    jo["settings"] = "Sale";
    jo["fullscreen"] = "";
    ja.append(jo);
#else
    jo = QJsonObject();
    jo["name"] = "Archive";
    jo["waiter_server"] = "";
    jo["host"] = "valsh.picassocloud.com/info.php";
    jo["database"] = "https://valsh.picassocloud.com/";
    jo["username"] = "";
    jo["password"] = "";
    jo["settings"] = "Sale";
    jo["fullscreen"] = "";
    ja.append(jo);
#endif
#endif

#ifdef REMOTE_ELINA
#ifdef QT_DEBUG
    jo = QJsonObject();
    jo["name"] = "ValShin";
    jo["waiter_server"] = "";
    jo["host"] = "127.0.0.1/engine/info.php";
    jo["database"] = "https://127.0.0.1/";
    jo["username"] = "";
    jo["password"] = "";
    jo["settings"] = "Sale";
    jo["fullscreen"] = "";
    ja.append(jo);
#else
    jo = QJsonObject();
    jo["name"] = "Archive";
    jo["waiter_server"] = "";
    jo["host"] = "aws.elina.am/info.php";
    jo["database"] = "https://aws.elina.am/";
    jo["username"] = "";
    jo["password"] = "";
    jo["settings"] = "GevorArtur";
    jo["fullscreen"] = "";
    ja.append(jo);
#endif
#endif

#ifdef REMOTE_DEBUG
    jo = QJsonObject();
    jo["name"] = "ValShin";
    jo["waiter_server"] = "";
    jo["host"] = "127.0.0.1/engine/info.php";
    jo["database"] = "https://127.0.0.1/";
    jo["username"] = "";
    jo["password"] = "";
    jo["settings"] = "Sale";
    jo["fullscreen"] = "";
    ja.append(jo);
#endif

#ifdef REMOTE_ALL
    QString configFile = path + "/servername.ini";
    if (!QFile::exists(configFile)) {
        LogWriter::write(LogWriterLevel::errors, "", QString("sqlquery config path not exists: %1").arg(configFile));
        jh["message"] = "Server not configured";
        return rh.setInternalServerError(jh.toString());
    }
    QFile f(configFile);
    f.open(QIODevice::ReadOnly);
    QJsonObject jconf = QJsonDocument::fromJson(f.readAll()).object();
    f.close();
    ja.append(jconf[jo["params"].toObject()["name"].toString()].toObject());
#endif

    return rh.setResponse(HTTP_OK, QJsonDocument(ja).toJson(QJsonDocument::Compact));
}
