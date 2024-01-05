#include "requestmanager.h"
#include "logwriter.h"
#include "database.h"
#include "commandline.h"
#include <QMutex>
#include <QElapsedTimer>
#include <QDir>
#include <QApplication>
#include <QLibrary>

static QMutex fMutex;

RequestManager *RequestManager::fInstance = 0;

RequestManager::RequestManager()
{

}

void RequestManager::init()
{
    fInstance = new RequestManager();
    QDir dir(qApp->applicationDirPath() + "/handlers");
    QStringList dll = dir.entryList(QStringList() << "*.dll", QDir::Files);
    for (auto &s: dll) {
        QLibrary *l = new QLibrary(qApp->applicationDirPath() + "/handlers/" + s);
        if (!l->load()) {
            LogWriter::write(LogWriterLevel::errors, "", QString("Did not load %1 %2").arg(s, l->errorString()));
            l->deleteLater();
            continue;
        }
        dllroutes dr = reinterpret_cast<dllroutes>(l->resolve("routes"));
        if (!dr) {
            l->deleteLater();
            continue;
        }
        QStringList sl;
        dr(sl);
        if (sl.count() == 0) {
            l->deleteLater();
            continue;
        }
        for (const QString &r: sl) {
            LogWriter::write(LogWriterLevel::verbose, "", QString("Loaded route %1").arg(r));
            fInstance->fRouteDll[r] = s;
            handle_route h = reinterpret_cast<handle_route>(l->resolve(r.toLatin1().data()));
            fInstance->fRouteFunction.insert(r, h);
        }
    }
}

handle_route RequestManager::getRouteHandler(const QString &route)
{
    if (fInstance->fRouteFunction.contains(route)) {
        return fInstance->fRouteFunction[route];
    }
    return 0;
}

void RequestManager::handle(const QString &session, const QString &remoteHost, const QString &r, const QByteArray &indata, QByteArray &outdata, const QHash<QString, DataAddress> &dataMap, ContentType contentType)
{
    handle_route hr = getRouteHandler(r);
    if (!hr) {
        hr = getRouteHandler("notfound");
        if (hr) {
            QByteArray ba = r.toUtf8();
            hr(ba, outdata, dataMap, ContentType::TextHtml);
            return;
        } else {
            outdata = "SERVER NOT CONFIGURE";
            return;
        }
    }
    QElapsedTimer et;
    et.start();
    LogWriter::write(LogWriterLevel::verbose, session, QString("Start handle route %1").arg(r));
    hr(indata, outdata, dataMap, contentType);

    int ms = et.elapsed();

    CommandLine cl;
    QString path;
    cl.value("dllpath", path);
    QString configFile = path + "/config.ini";
    Database db;
    if (db.open(configFile)) {
        db[":fdate"] = QDate::currentDate();
        db[":ftime"] = QTime::currentTime();
        db[":fhost"] = remoteHost;
        db[":felapsed"] = ms;
        db[":froute"] = r;
        db[":frequest"] = QString::fromUtf8(indata);
        db[":fresponse"] = QString::fromUtf8(outdata);
        db.insert("system_requests");
    } else {
        LogWriter::write(LogWriterLevel::errors, session, db.lastDbError());
    }
}
