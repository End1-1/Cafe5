#include "datadriver.h"
#include "dlgscreen.h"
#include "c5systempreference.h"
#include "fileversion.h"
#include "c5config.h"
#include "c5database.h"
#include "c5servername.h"
#include "ndataprovider.h"
#include "dlgserverconnection.h"
#include "c5message.h"
#include <QApplication>
#include <QTranslator>
#include "dlgsplashscreen.h"
#include <QFile>
#include <QDir>
#include <QLockFile>
#include <QMessageBox>
#include <QSettings>

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
    qputenv("QT_ASSUME_UTF8", "1");
    QFile styleSheet(a.applicationDirPath() + "/waiter.css");
    if (styleSheet.open(QIODevice::ReadOnly)) {
        a.setStyleSheet(styleSheet.readAll());
        styleSheet.close();
    }
    QDir d;
    QFile file(d.homePath() + "/" + _APPLICATION_ + "/" + _MODULE_ + ".lock.pid");
    file.remove();
    QLockFile lockFile(d.homePath() + "/" + _APPLICATION_  + "/" + _MODULE_ + ".lock.pid");
    if (!lockFile.tryLock()) {
        C5Message::error(QObject::tr("An instance of application already running"));
        return -1;
    }
    auto *dlgsplash = new DlgSplashScreen();
    dlgsplash->show();
    emit dlgsplash->messageSignal("Get server name");
    auto sng = new C5ServerName(__c5config.getRegValue("ss_server_address").toString());
    if (!sng->getConnection(__c5config.getRegValue("ss_database").toString())) {
        C5Message::error(sng->mErrorString);
        DlgServerConnection::showSettings(0);
        return 1;
    }
    QJsonObject js = sng->mReply;
    sng->deleteLater();
    C5Config::fDBHost = js["settings"].toString();
    C5Config::fDBPath = js["database"].toString();
    C5Config::fSettingsName = __c5config.getRegValue("ss_settings").toString();
#ifdef QT_DEBUG
    C5Config::fFullScreen = false;
#else
    C5Config::fFullScreen = true;
#endif
    C5Config::initParamsFromDb();
    C5Database::LOGGING = C5Config::getValue(param_debuge_mode).toInt() == 1;
    NDataProvider::mProtocol = js["settings"].toString();
    if (!C5SystemPreference::checkDecimalPointAndSeparator()) {
        return 0;
    }
    QTranslator t;
    if (t.load(":/Waiter.qm")) {
        a.installTranslator( &t);
    }
    QFont font(a.font());
    font.setFamily("Arial LatArm Unicode");
    font.setPointSize(11);
    a.setFont(font);
    C5Database db(__c5config.dbParams());
    DbData::setDBParams(__c5config.dbParams());
    DataDriver::fInstance = new DataDriver();
    DataDriver::norefresh.append("goods");
    DataDriver::init(__c5config.dbParams(), dlgsplash);
    NDataProvider::mAppName = "waiter";
    NDataProvider::mFileVersion = FileVersion::getVersionString(qApp->applicationFilePath());
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
