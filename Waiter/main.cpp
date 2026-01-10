#include "dlgscreen.h"
#include "c5systempreference.h"
#include "fileversion.h"
#include "c5connectiondialog.h"
#include "ndataprovider.h"
#include "logwriter.h"
#include "c5message.h"
#include "c5user.h"
#include "printtaxn.h"
#include "dlgsplashscreen.h"
#include <QTimer>
#include <QApplication>
#include <QTranslator>
#include <QFile>
#include <QDir>
#include <QLockFile>
#include <QMessageBox>
#include <QSettings>
#include <QScreen>

int main(int argc, char* argv[])
{
#ifdef QT_DEBUG
    QGuiApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_Use96Dpi);
#endif
    QApplication a(argc, argv);
    LogWriter::write(LogWriterLevel::verbose, "Support SSL", QSslSocket::supportsSsl() ? "true" : "false");
    LogWriter::write(LogWriterLevel::verbose, "Support SSL version", QSslSocket::sslLibraryBuildVersionString());
#ifndef QT_DEBUG
    QStringList libPath;
    libPath << qApp->applicationDirPath();
    libPath << qApp->applicationDirPath() + "/platforms";
    libPath << qApp->applicationDirPath() + "/sqldrivers";
    libPath << qApp->applicationDirPath() + "/printsupport";
    libPath << qApp->applicationDirPath() + "/imageformats";
    QCoreApplication::setLibraryPaths(libPath);
#endif
    qputenv("QT_ASSUME_UTF8", "1");
    QFile styleSheet(a.applicationDirPath() + "/waiter.css");

    if(styleSheet.open(QIODevice::ReadOnly)) {
        a.setStyleSheet(styleSheet.readAll());
        styleSheet.close();
    }

    QDir d;
    QFile file(d.homePath() + "/" + _APPLICATION_ + "/" + _MODULE_ + ".lock.pid");
    file.remove();
    QLockFile lockFile(d.homePath() + "/" + _APPLICATION_  + "/" + _MODULE_ + ".lock.pid");

    if(!lockFile.tryLock()) {
        C5Message::error(QObject::tr("An instance of application already running"));
        return -1;
    }

    for(const QString &s : a.arguments()) {
        if(s.startsWith("/monitor")) {
            QList<QScreen*> screens = a.screens();
            int monitor = 0;
            QStringList mon = s.split("=");

            if(mon.length() == 2) {
                monitor = mon.at(1).toInt();
                C5Dialog::mScreen = monitor;
            }
        }
    }

    auto *dlgsplash = new DlgSplashScreen(nullptr);
    dlgsplash->show();
    emit dlgsplash->messageSignal("Get server name");

    if(!C5SystemPreference::checkDecimalPointAndSeparator()) {
        return 0;
    }

    QTranslator t;

    if(t.load(":/Waiter.qm")) {
        a.installTranslator(&t);
    }

    QString fileVersion =  FileVersion::getVersionString(a.applicationFilePath());
    int build = 0;
    const QStringList parts = fileVersion.split('.');

    if(parts.size() >= 4) {
        build = parts.at(3).toInt();
    }

    PrintTaxN::mDebugRseq = build;
    QFont font(a.font());
    font.setFamily("Arial LatArm Unicode");
    font.setPointSize(11);
    a.setFont(font);
    NDataProvider::mAppName = "waiter";
    NDataProvider::mFileVersion = fileVersion;
    NDataProvider::mProtocol = C5ConnectionDialog::instance()->noneSecure ? "http" : "https";
    NDataProvider::mHost = C5ConnectionDialog::instance()->serverAddress();
    NDataProvider::mDebug = a.arguments().contains("/debug");
    dlgsplash->login(C5ConnectionDialog::instance()->username(),
                     C5ConnectionDialog::instance()->password(),
    [dlgsplash](C5User * user) {
        dlgsplash->deleteLater();
        C5Dialog::setMainWindow(nullptr);
        QTimer::singleShot(1, [user]() {
            auto *w = new DlgScreen(user);
            w->exec();
        });
    });
    return a.exec();
}
