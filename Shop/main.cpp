#include <QApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFontDatabase>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLockFile>
#include <QMessageBox>
#include <QScreen>
#include <QSettings>
#include <QStyleFactory>
#include <QTranslator>
#include "c5connectiondialog.h"
#include "c5dialog.h"
#include "c5message.h"
#include "c5systempreference.h"
#include "c5user.h"
#include "dict_workstation.h"
#include "dlgpin.h"
#include "dlgsplashscreen.h"
#include "fileversion.h"
#include "logwriter.h"
#include "ndataprovider.h"
#include "printtaxn.h"
#include "struct_workstationitem.h"
#include "working.h"

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

    a.setStyle(QStyleFactory::create("fusion"));
    QFile styleSheet(a.applicationDirPath() + "/shop.css");

    if(styleSheet.exists()) {
        if (styleSheet.open(QIODevice::ReadOnly)) {
            a.setStyleSheet(styleSheet.readAll());
        }
    }

    auto *t = new QTranslator();

    if (t->load(":/lang/Shop.qm")) {
        a.installTranslator(t);
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
    bool multicopy = false;

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

        if (s.startsWith("/multicopy")) {
            multicopy = true;
        }

        if (s.startsWith("/settingspath")) {
            QStringList ver = s.split("=");

            if (ver.length() == 2) {
                C5ConnectionDialog::mSettingsPath = ver.at(1);
            }
        }
    }

    if (!multicopy) {
        QLockFile lockFile(d.homePath() + "/" + _APPLICATION_ + "/lock.pid");

        if (!lockFile.tryLock()) {
            C5Message::error(QObject::tr("An instance of application already running"));
            return -1;
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
    auto *dlgsplash = new DlgSplashScreen(WORKSTATION_SHOP, user);
    dlgsplash->mOnFinish = [dlgsplash](C5User *user) {
        emit dlgsplash->messageSignal("init data driver...");
        C5Dialog::setMainWindow(nullptr);

        auto *w = new Working(user);
        w->setWindowTitle("");

        if (mWorkStation.defaultHallId() > 0) {
            w->setWindowTitle(w->windowTitle() + "[" + mWorkStation.defaultHallName() + "]");
        }

        if (mWorkStation.defaultStoreId() > 0) {
            w->setWindowTitle(w->windowTitle() + "[" + mWorkStation.defaultStoreName() + "]");
        } else {
            C5Message::error(QObject::tr("Store is not defined."));
            qApp->quit();
        }
        dlgsplash->hide();
        dlgsplash->deleteLater();
        w->showMaximized();
    };
    dlgsplash->show();
    dlgsplash->prepare();

    return a.exec();
}
