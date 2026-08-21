#include "sqlquery.h"
#include "logwriter.h"
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace {

QString handlersDir()
{
    return QCoreApplication::applicationDirPath() + QLatin1String("/handlers");
}

bool loadServerNameConfig(QJsonObject &jconf, QString &err)
{
    const QString configFile = handlersDir() + QLatin1String("/servername.ini");
    if (!QFile::exists(configFile)) {
        LogWriter::write(LogWriterLevel::errors, "",
                         QString("sqlquery config path not exists: %1").arg(configFile));
        err = "Server not configured";
        return false;
    }
    QFile f(configFile);
    if (!f.open(QIODevice::ReadOnly)) {
        err = "Server not configured";
        return false;
    }
    jconf = QJsonDocument::fromJson(f.readAll()).object();
    return true;
}

}

bool office(const QJsonObject &jreq, QJsonObject &jret, QString &err)
{
    qDebug() << "office request" << jreq;
    if (jreq["key"].toString() != "asdf7fa8kk49888d!!jjdjmskkak98983mj???m") {
        err = "Unauthorized";
        return false;
    }
    QJsonArray ja;
    QJsonObject jo;
#ifdef REMOTE_ELINA
    jo = QJsonObject();
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
    QJsonObject jconf;
    if (!loadServerNameConfig(jconf, err)) {
        return false;
    }
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
    QJsonObject jo;
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
    QJsonObject jconf;
    if (!loadServerNameConfig(jconf, err)) {
        return false;
    }
    const QString name = jreq["params"].toObject()["name"].toString();
    if (!jconf.contains(name)) {
        err = QString("Params %1 not declare %2").arg(name, QJsonDocument(jconf).toJson());
        return false;
    }
    ja.append(jconf[name].toObject());
#endif
    jret["result"] = ja;
    return true;
}
