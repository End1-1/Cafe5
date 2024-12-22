#include "c5sockethandler.h"
#include "datadriver.h"
#include "dlgscreen.h"
#include "c5systempreference.h"
#include "c5servername.h"
#include "ndataprovider.h"
#include <QApplication>
#include <QTranslator>
#include "dlgsplashscreen.h"
#include <QFile>
#include <QDir>
#include <QLockFile>
#include <QMessageBox>
#include <QSettings>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#ifndef QT_DEBUG
    QStringList libPath;
    libPath << qApp->applicationDirPath();
    libPath << qApp->applicationDirPath() + "/platforms";
    libPath << qApp->applicationDirPath() + "/sqldrivers";
    libPath << qApp->applicationDirPath() + "/printsupport";
    libPath << qApp->applicationDirPath() + "/imageformats";
    QCoreApplication::setLibraryPaths(libPath);
#endif
    QFile styleSheet(a.applicationDirPath() + "/waiter.css");
    if (styleSheet.open(QIODevice::ReadOnly)) {
        a.setStyleSheet(styleSheet.readAll());
        styleSheet.close();
    }
    QDir d;
    QFile file(d.homePath() + "/" + _APPLICATION_ + "/lock.pid");
    file.remove();
    QLockFile lockFile(d.homePath() + "/" + _APPLICATION_ + "/lock.pid");
    if (!lockFile.tryLock()) {
        C5Message::error(QObject::tr("An instance of application already running"));
        return -1;
    }
    auto *dlgsplash = new DlgSplashScreen();
    dlgsplash->show();
    emit dlgsplash->messageSignal("Get server name");
    QString servername, configname, params = "waiter";
    for (const QString &s : a.arguments()) {
        if (s.contains("/servername")) {
            QStringList sn = s.split("=", Qt::SkipEmptyParts);
            if (sn.length() == 2) {
                servername = sn.at(1);
            }
        } else if (s.contains("/config")) {
            QStringList sn = s.split("=", Qt::SkipEmptyParts);
            if (sn.length() == 2) {
                configname = sn.at(1);
            }
        } else if (s.contains("/params")) {
            QStringList sn  = s.split("=", Qt::SkipEmptyParts);
            if (sn.length() == 2) {
                params = sn.at(1);
            }
        }
    }
    QString err;
    if (servername.isEmpty()) {
        err += "Arguments must contains server name<br>";
    }
    if (err.isEmpty() == false) {
        C5Message::error(err);
        return 1;
    }
    C5ServerName sn(servername, "shop");
    sn.mParams["workstation"] = hostinfo;
    if (!sn.getServers(params)) {
        C5Message::error(sn.mErrorString);
        return 1;
    }
    if (sn.mServers.size() == 0) {
        C5Message::error("No configuration for this station");
        return 1;
    }
    QJsonObject js = sn.mServers.at(0).toObject();
    if (configname.isEmpty() == false) {
        js["settings"] = configname;
    }
    C5Config::fDBHost = js["database"].toString();
    C5Config::fDBPath = js["database"].toString();
    C5Config::fDBUser = js["username"].toString();
    C5Config::fDBPassword = js["password"].toString();
    C5Config::fSettingsName = js["settings"].toString();
    C5Config::fFullScreen = js["fullscreen"].toBool();
    C5SocketHandler::setServerAddress(js["waiter_server"].toString());
    C5Config::initParamsFromDb();
    C5Database::LOGGING = C5Config::getValue(param_debuge_mode).toInt() == 1;
    if (!C5SystemPreference::checkDecimalPointAndSeparator()) {
        return 0;
    }
    QTranslator t;
    t.load(":/Waiter.qm");
    a.installTranslator( &t);
    QFont font(a.font());
    font.setFamily("Arial LatArm Unicode");
    font.setPointSize(11);
    a.setFont(font);
    C5Database db(__c5config.dbParams());
    if (db.open()) {
        DbData::setDBParams(__c5config.dbParams());
        DataDriver::fInstance = new DataDriver();
        DataDriver::norefresh.append("goods");
        DataDriver::init(__c5config.dbParams(), dlgsplash);
    } else {
        C5Message::error(db.fLastError);
    }
    NDataProvider::mHost = C5Config::fDBPath;
    NDataProvider::mDebug = C5Config::getValue(param_debuge_mode).toInt() > 0;
    dlgsplash->deleteLater();
    DlgScreen w;
    C5Config::fParentWidget = &w;
    if (C5Config::isAppFullScreen()) {
        w.setWindowState(Qt::WindowFullScreen);
    }
    w.show();
    a.processEvents();
    return a.exec();
}
