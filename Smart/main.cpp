#include "workspace.h"
#include "c5config.h"
#include "c5systempreference.h"
#include "datadriver.h"
#include "logwriter.h"
#include "c5servername.h"
#include <QFile>
#include <QLockFile>
#include <QSslSocket>
#include <QApplication>
#include <QSettings>
#include <QTranslator>
#include <QDir>

int main(int argc, char *argv[])
{

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
    QFile file(d.homePath() + "/" + _APPLICATION_ + "/lock.pid");
    file.remove();
    QLockFile lockFile(d.homePath() + "/" + _APPLICATION_ + "/lock.pid");
    if (!lockFile.tryLock()) {
        C5Message::error(QObject::tr("An instance of application already running"));
        return -1;
    }

    LogWriter::write(LogWriterLevel::verbose, "", "OpenSSL version " + QSslSocket::sslLibraryBuildVersionString() + " " + QSslSocket::sslLibraryVersionString());
    LogWriter::write(LogWriterLevel::verbose, "", "OpenSSL support " + QString((QSslSocket::supportsSsl() ? "YES" : "NO")));

    QTranslator t;
    t.load(":/Smart.qm");
    a.installTranslator(&t);
    QStringList args;
    for (int i = 0; i < argc; i++) {
        args << argv[i];
    }

    QFont f("Arial LatArm Unicode", 10);
    //QFont f(__c5config.getValue(param_app_font_family), 10);
    qApp->setFont(f);
    QFile styleFile(qApp->applicationDirPath() + "/smartstyle.css");
    if (styleFile.open(QIODevice::ReadOnly)) {
        a.setStyleSheet(styleFile.readAll());
    }

    QString settingsName, serverName;
    for (const QString &arg: a.arguments()) {
        if (arg.contains("/servername=")){
            QStringList sn = arg.split("=");
            if (sn.length() == 2) {
                serverName = sn.at(1);
                C5ServerName sng(serverName, "shop");
                if (!sng.getServers("smart")){
                    return 1;
                }
                QJsonObject js = sng.mServers.at(0).toObject();
                C5Config::fDBHost = js["host"].toString();
                C5Config::fDBPath = js["database"].toString();
                C5Config::fDBUser = js["username"].toString();
                C5Config::fDBPassword = js["password"].toString();
                C5Config::fSettingsName = js["settings"].toString();
                C5Config::fFullScreen = true;
                QSettings ss(_ORGANIZATION_, _APPLICATION_+ QString("\\") + _MODULE_);
                ss.setValue("server", "");
            }
        } else if (arg.contains("/config")) {
            QStringList sn = arg.split("=", Qt::SkipEmptyParts);
            if (sn.length() > 1) {
                settingsName = sn.at(1);
                C5Config::fSettingsName = settingsName;
            }
        }
    }
    if (serverName.isEmpty()) {
        C5Message::error("Connection server name not found");
        return 1;
    }

    C5Config::initParamsFromDb();
    DataDriver::init(C5Config::dbParams());

    if (!C5SystemPreference::checkDecimalPointAndSeparator()) {
        return 0;
    }
    Workspace w(C5Config::dbParams());
    C5Config::fParentWidget = &w;
#ifdef QT_DEBUG
    C5Database::LOGGING = true;
#endif
    if (C5Config::getValue(param_debuge_mode).toInt() > 0) {
        C5Database::LOGGING = true;
    }
    w.showFullScreen();
    a.processEvents();
    if (!w.login()) {
        return 0;
    }

    return a.exec();
}
