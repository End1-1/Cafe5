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
#include "c5registrysettings.h"
#include "c5connectiondialog.h"
#include "c5dialog.h"
#include "c5message.h"
#include "c5systempreference.h"
#include "c5uilanguage.h"
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

        if (s.startsWith("/settingspath", Qt::CaseInsensitive)) {
            const int eq = s.indexOf(QLatin1Char('='));
            if (eq > 0) {
                const QString subPath = s.mid(eq + 1).trimmed();
                //C5Config::fSettingsSubPath = subPath;
                C5ConnectionDialog::mSettingsPath = subPath;
                C5RegistrySettings::settingsSubPath = subPath;
            }
        }
    }

    C5UiLanguage::configure(QStringLiteral(":/lang/Shop.qm"),
                            QStringLiteral(":/lang/Shop_ru.qm"));
    C5UiLanguage::loadSaved();

    const QString appHome = d.homePath() + "/" + _APPLICATION_;
    d.mkpath(appHome);
    d.mkpath(appHome + "/logs");

    // Must stay in scope for the whole process lifetime — otherwise the lock is released
    // immediately and a leftover/stale lock.pid (or missing dir) falsely blocks startup.
    QLockFile lockFile(appHome + "/" + _MODULE_ + ".lock.pid");

    if (!multicopy) {
        lockFile.setStaleLockTime(0);
        lockFile.removeStaleLockFile();

        if (!lockFile.tryLock()) {
            if (lockFile.error() == QLockFile::LockFailedError) {
                C5Message::error(QObject::tr("An instance of application already running"));
            } else {
                C5Message::error(QObject::tr("Cannot create lock file: %1").arg(lockFile.fileName()));
            }

            return -1;
        }
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
            C5Message::error(QObject::tr("Store is not defined.\nWorkstation: %1\nAccount: %2\nSet f_default_store_id in workstation settings.")
                                 .arg(mWorkStation.name, mWorkStation.stationAccount));
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
