#include "sqlquery.h"
#include "logwriter.h"
#include "database.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

bool office(const QJsonObject &jreq, QJsonObject &jret, QString &err)
{
    qDebug() << "office request" << jreq;
    if (jreq["key"].toString() != "asdf7fa8kk49888d!!jjdjmskkak98983mj???m") {
        err = "Unauthorized";
        return false;
    }
    QJsonArray ja;
#ifdef REMOTE_ELINA
    QJsonObject jo = QJsonObject();
    jo["name"] = "Archive";
    jo["waiter_server"] = "";
    jo["host"] = "e3.picasso.am/info.php";
    jo["database"] = "https://e3.picasso.am/";
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
    jo = QJsonObject();
    jo["name"] = "Elina";
    jo["waiter_server"] = "";
    jo["host"] = "e3.picasso.am/info.php";
    jo["database"] = "https://e3.picasso.am/";
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
        err = "Server not configured";
        return false;
    }
    QFile f(configFile);
    f.open(QIODevice::ReadOnly);
    QJsonObject jconf = QJsonDocument::fromJson(f.readAll()).object();
    f.close();
    ja = jconf[jreq["params"].toObject()["name"].toString()].toArray();
#endif
    jret["result"] = ja;
    return true;
}

bool shop(const QJsonObject &jreq, QJsonObject &jret, QString &err)
{
    qDebug() << "office request" << jreq;
    if (jreq["key"].toString() != "asdf7fa8kk49888d!!jjdjmskkak98983mj???m") {
        err = "Unauthorized";
        return false;
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
    QJsonObject jo = QJsonObject();
    jo["name"] = "Archive";
    jo["waiter_server"] = "";
    jo["host"] = "e3.picasso.am/info.php";
    jo["database"] = "https://e3.picasso.am/";
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
        err = "Server not configured";
        return false;
    }
    QFile f(configFile);
    f.open(QIODevice::ReadOnly);
    QJsonObject jconf = QJsonDocument::fromJson(f.readAll()).object();
    f.close();
    if (!jconf.contains(jreq["params"].toObject()["name"].toString())) {
        err = QString("Params %1 not declare %2").arg(jreq["params"].toObject()["name"].toString(),
              QJsonDocument(jconf).toJson());
        return false;
    }
    ja.append(jconf[jreq["params"].toObject()["name"].toString()].toObject());
#endif
    jret["result"] = ja;
    return true;
}
