#include "workspace.h"
#include "c5config.h"
#include "c5systempreference.h"
#include "ndataprovider.h"
#include "c5servername.h"
#include "ndataprovider.h"
#include "dlgsplashscreen.h"
#include "fileversion.h"
#include "logwriter.h"
#include "c5message.h"
#include "c5database.h"
#include "dlgserverconnection.h"
#include <QFile>
#include <QLockFile>
#include <QSslSocket>
#include <QApplication>
#include <QSettings>
#include <QTranslator>
#include <QScreen>
#include <QDesktopServices>
#include <QDir>

int main(int argc, char* argv[])
{
    qputenv("QT_OPENGL", "angle");
    QApplication a(argc, argv);
#ifndef QT_DEBUG
    QStringList libPath = QCoreApplication::libraryPaths();
    libPath << a.applicationDirPath();
    libPath << a.applicationDirPath() + "/platforms";
    libPath << a.applicationDirPath() + "/sqldrivers";
    libPath << a.applicationDirPath() + "/printsupport";
    libPath << a.applicationDirPath() + "/imageformats";
    QCoreApplication::setLibraryPaths(libPath);
#endif
    //    if (QDate::currentDate() > QDate::fromString("28/02/2023", "dd/MM/yyyy")) {
    //        return 1;
    //    }
    QDir d;
    QFile file(d.homePath() + "/" + _APPLICATION_ + "/" + _MODULE_ + "lock.pid");
    file.remove();
    QLockFile lockFile(d.homePath() + "/" + _APPLICATION_ + "/" + _MODULE_ + "lock.pid");

    if(!lockFile.tryLock()) {
        C5Message::error(QObject::tr("An instance of application already running"));
        return -1;
    }

    LogWriter::write(LogWriterLevel::verbose, "Support SSL", QSslSocket::supportsSsl() ? "true" : "false");
    LogWriter::write(LogWriterLevel::verbose, "Support SSL version", QSslSocket::sslLibraryBuildVersionString());
    auto *dlgsplash = new DlgSplashScreen();
    dlgsplash->show();
    emit dlgsplash->messageSignal("Please, wait...");
    QTranslator t;

    if(t.load(":/Smart.qm")) {
        a.installTranslator(&t);
    }

    QStringList args;

    for(int i = 0; i < argc; i++) {
        args << argv[i];
    }

    QFont f("Arial LatArm Unicode", 10);
    //QFont f(__c5config.getValue(param_app_font_family), 10);
    qApp->setFont(f);
    QFile styleFile(qApp->applicationDirPath() + "/smartstyle.css");

    if(styleFile.open(QIODevice::ReadOnly)) {
        a.setStyleSheet(styleFile.readAll());
    }

    C5ServerName sng(__c5config.getRegValue("ss_server_address").toString());

    if(!sng.getConnection(__c5config.getRegValue("ss_database").toString())) {
        C5Message::error(sng.mErrorString);
        DlgServerConnection::showSettings(0);
        return 1;
    }

    QJsonObject js = sng.mReply;
    C5Config::fDBHost = js["settings"].toString();
    C5Config::fDBPath = js["database"].toString();
    C5Config::fDBUser = js["username"].toString();
    C5Config::fDBPassword = js["password"].toString();
    C5Database::fDbParams = C5Config::dbParams();
    C5Config::fSettingsName = __c5config.getRegValue("ss_settings").toString();
    C5Config::fFullScreen = true;
#ifdef QT_DEBUG
    C5Config::fFullScreen = false;
#endif
    NDataProvider::mProtocol = C5Config::fDBHost;
    NDataProvider::mHost = C5Config::fDBPath;
    NDataProvider::mAppName = "smart";
    NDataProvider::mFileVersion = FileVersion::getVersionString(a.applicationFilePath());
    QSettings ss(_ORGANIZATION_, _APPLICATION_ + QString("\\") + _MODULE_);
    ss.setValue("server", "");
    C5Config::initParamsFromDb();

    if(!C5SystemPreference::checkDecimalPointAndSeparator()) {
        return 0;
    }

    Workspace w;

    for(const QString &s : args) {
        if(s.startsWith("/monitor")) {
            QList<QScreen*> screens = a.screens();
            int monitor = 0;
            QStringList mon = s.split("=");

            if(mon.length() == 2) {
                monitor = mon.at(1).toInt();
            }

            w.move(screens.at(monitor)->geometry().topLeft());
        }
    }

    C5Config::fParentWidget = &w;
#ifdef QT_DEBUG
    C5Database::LOGGING = true;
#endif

    if(C5Config::getValue(param_debuge_mode).toInt() > 0) {
        C5Database::LOGGING = true;
        NDataProvider::mDebug = true;
    }

    dlgsplash->hide();
    dlgsplash->deleteLater();
    w.showFullScreen();
    w.login();
    return a.exec();
}
