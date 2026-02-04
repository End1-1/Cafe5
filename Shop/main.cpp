#include "c5config.h"
#include "dlgpin.h"
#include "ndataprovider.h"
#include "datadriver.h"
#include "printtaxn.h"
#include "c5dialog.h"
#include "c5systempreference.h"
#include "c5database.h"
#include "c5message.h"
#include "dlgsplashscreen.h"
#include "fileversion.h"
#include "logwriter.h"
#include "working.h"
#include "c5user.h"
#include <QApplication>
#include <QMessageBox>
#include <QLockFile>
#include <QFontDatabase>
#include <QInputDialog>
#include <QDir>
#include <QTranslator>
#include <QStyleFactory>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QScreen>
#include <QDesktopServices>
#include <QSettings>

int main(int argc, char* argv[])
{
#ifndef QT_DEBUG
    QStringList libPath = QCoreApplication::libraryPaths();
    libPath << qApp->applicationDirPath();
    libPath << qApp->applicationDirPath() + "/platforms";
    libPath << qApp->applicationDirPath() + "/sqldrivers";
    libPath << qApp->applicationDirPath() + "/printsupport";
    libPath << qApp->applicationDirPath() + "/imageformats";
    libPath << qApp->applicationDirPath() + "/plugins";
    QCoreApplication::setLibraryPaths(libPath);
#endif
    QApplication a(argc, argv);
    qputenv("QT_ASSUME_UTF8", "1");
    LogWriter::write(LogWriterLevel::verbose, "Support SSL", QSslSocket::supportsSsl() ? "true" : "false");
    LogWriter::write(LogWriterLevel::verbose, "Support SSL version", QSslSocket::sslLibraryBuildVersionString());
    QDir d;
    QLockFile lockFile(d.homePath() + "/" + _APPLICATION_ + "/lock.pid");

    if(!lockFile.tryLock()) {
        C5Message::error(QObject::tr("An instance of application already running"));
        return -1;
    }

    // LogWriter::write(LogWriterLevel::verbose, "", "starting...");
    a.setStyle(QStyleFactory::create("fusion"));
    QFile styleSheet(a.applicationDirPath() + "/shop.css");

    if(styleSheet.exists()) {
        if (styleSheet.open(QIODevice::ReadOnly)) {
            a.setStyleSheet(styleSheet.readAll());
        }
    }

    QTranslator t;

    if(t.load(":/lang/Shop.qm")) {
        a.installTranslator(&t);
    }

    QString fileVersion =  FileVersion::getVersionString(a.applicationFilePath());
    int build = 0;
    const QStringList parts = fileVersion.split('.');

    if(parts.size() >= 4) {
        build = parts.at(3).toInt();
    }

    PrintTaxN::mDebugRseq = build;
    bool debug = false;
    bool donotautologin = false;

    for(const QString &s : a.arguments()) {
        if(s.startsWith("/monitor")) {
            QList<QScreen*> screens = a.screens();
            int monitor = 0;
            QStringList mon = s.split("=");

            if(mon.length() == 2) {
                monitor = mon.at(1).toInt();
            }

            if(screens.count() > monitor - 1) {
                C5Dialog::mScreen = monitor;
            }
        }

        if(s.startsWith("/setversion")) {
            QStringList ver = s.split("=");

            if(ver.length() == 2) {
                fileVersion = ver.at(1);
            }
        }

        if(s.startsWith("/debug")) {
            debug = true;
        }

        if(s.startsWith("/noautologin")) {
            donotautologin = true;
        }
    }

    if(!d.exists(d.homePath() + "/" + _APPLICATION_)) {
        d.mkpath(d.homePath() + "/" + _APPLICATION_);
    }

    if(!d.exists(d.homePath() + "/" + _APPLICATION_ + "/logs")) {
        d.mkpath(d.homePath() + "/" + _APPLICATION_ + "/logs");
    }

    if(!C5SystemPreference::checkDecimalPointAndSeparator()) {
        return 0;
    }

    NDataProvider::mAppName = "shop";
    NDataProvider::mFileVersion = fileVersion;
    NDataProvider::mDebug = debug;
    auto *dlgPin = new DlgPin();
    dlgPin->mDoNotAutoLogin = donotautologin;

    if(dlgPin->exec() == QDialog::Rejected) {
        return 0;
    }

    auto *user = new C5User(dlgPin->mUser);
    auto *dlgsplash = new DlgSplashScreen(nullptr);
    dlgsplash->show();
    C5Database db;
    emit dlgsplash->messageSignal("init data driver...");
    DataDriver::init(__c5config.dbParams(), dlgsplash);
    C5Dialog::setMainWindow(nullptr);
    auto *w = new Working(user);
    w->setWindowTitle("");

    if(__c5config.defaultHall() > 0) {
        w->setWindowTitle(w->windowTitle() + "[" + __c5config.fMainJson["shop_hall_name"].toString() + "]");
    }

    if(__c5config.defaultStore() > 0) {
        w->setWindowTitle(w->windowTitle() + "[" + dbstore->name(__c5config.defaultStore()) + "]");
    } else {
        C5Message::error(QObject::tr("Store is not defined."));
        qApp->quit();
    }

    if(__c5config.fMainJson["clear_sale_draft"].toBool()) {
        emit dlgsplash->messageSignal("clear drafts...");
        db[":f_hall"] = C5Config::defaultHall();
        db.exec("update o_draft_sale_body set f_state=10 where f_state=1 "
                "and f_header in (select f_id from o_draft_sale where f_hall=:f_hall and f_state=1)");
        db[":f_hall"] = C5Config::defaultHall();
        db.exec("update o_draft_sale set f_state=10 where f_hall=:f_hall and f_state=1");
    }

    dlgsplash->hide();
    dlgsplash->deleteLater();
    __c5config.setRegValue("windowtitle", w->windowTitle());
    w->showMaximized();
    return a.exec();
}
