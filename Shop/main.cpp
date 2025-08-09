#include "working.h"
#include "c5config.h"
#include "dlgpin.h"
#include "dlgserverconnection.h"
#include "ndataprovider.h"
#include "datadriver.h"
#include "c5user.h"
#include "c5servername.h"
#include "settingsselection.h"
#include "c5dialog.h"
#include "c5systempreference.h"
#include "c5database.h"
#include "c5message.h"
#include "dlgsplashscreen.h"
#include "fileversion.h"
#include "logwriter.h"
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

    LogWriter::write(LogWriterLevel::verbose, "", "starting...");
    a.setStyle(QStyleFactory::create("fusion"));
    QFile styleSheet(a.applicationDirPath() + "/shop.css");

    if(styleSheet.exists()) {
        styleSheet.open(QIODevice::ReadOnly);
        a.setStyleSheet(styleSheet.readAll());
    }

    QTranslator t;

    if(t.load(":/lang/Shop.qm")) {
        a.installTranslator(&t);
    }

    auto *dlgsplash = new DlgSplashScreen();
    dlgsplash->show();
    LogWriter::write(LogWriterLevel::verbose, "", "get server names");
    emit dlgsplash->messageSignal("Get server name");
    bool noconfig = true;
    C5ServerName sng(__c5config.getRegValue("ss_server_address").toString());

    if(!sng.getConnection(__c5config.getRegValue("ss_database").toString())) {
        C5Message::error(sng.mErrorString);
        DlgServerConnection::showSettings(0);
        return 1;
    }

    noconfig = false;
    QJsonObject js = sng.mReply;
    C5Config::fDBHost = js["settings"].toString();
    C5Config::fDBPath = js["database"].toString();
    C5Config::fDBUser = js["username"].toString();
    NDataProvider::mProtocol = js["settings"].toString();
    C5Config::fDBPassword = js["password"].toString();
    C5Config::fSettingsName = __c5config.getRegValue("ss_settings").toString();
    C5Config::fFullScreen = true;
    QSettings ss(_ORGANIZATION_, _APPLICATION_ + QString("\\") + _MODULE_);
    ss.setValue("server", "");

    if(noconfig) {
        QMessageBox::critical(0, "Error", "No config");
        return 1;
    }

    if(!d.exists(d.homePath() + "/" + _APPLICATION_)) {
        d.mkpath(d.homePath() + "/" + _APPLICATION_);
    }

    if(!d.exists(d.homePath() + "/" + _APPLICATION_ + "/logs")) {
        d.mkpath(d.homePath() + "/" + _APPLICATION_ + "/logs");
    }

    QFontDatabase::addApplicationFont(":/barcode.ttf");
    //    int id = QFontDatabase::addApplicationFont(":/ahuni.ttf");
    //    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFile shopstyle(a.applicationDirPath() + "/shopstyle.qss");

    if(shopstyle.exists()) {
        if(shopstyle.open(QIODevice::ReadOnly)) {
            a.setStyleSheet(shopstyle.readAll());
        }
    }

    NDataProvider::mAppName = "shop";
    NDataProvider::mFileVersion = FileVersion::getVersionString(a.applicationFilePath());
    QString user, pin;
    LogWriter::write(LogWriterLevel::verbose, "", "Init params from db");
    emit dlgsplash->messageSignal("Init params from db");
    C5Database::fDbParams = C5Config::dbParams();
    C5Config::initParamsFromDb();
    NDataProvider::mDebug = __c5config.getValue(param_debuge_mode).toInt() > 0;
    C5Database db;
    __user = new C5User(0);

    while(!__user->isValid() || !__user->isActive()) {
        if(!DlgPin::getPin(user, pin, false)) {
            return 0;
        }

        LogWriter::write(LogWriterLevel::verbose, "", "try login to db");
        emit dlgsplash->messageSignal("try login to db");

        if(__user->authByPinPass(user, pin)) {
        } else {
            C5Message::error(__user->error() + " 2");
        }

        pin.clear();
        user.clear();
    }

    if(C5Config::fMainJson["clear_sale_draft"].toBool()) {
        emit dlgsplash->messageSignal("clear draft");
        db[":f_hall"] = C5Config::defaultHall();
        db.exec("update o_draft_sale_body set f_state=10 where f_state=1 "
                "and f_header in (select f_id from o_draft_sale where f_hall=:f_hall and f_state=1)");
        db[":f_hall"] = C5Config::defaultHall();
        db.exec("update o_draft_sale set f_state=10 where f_hall=:f_hall and f_state=1");
    }

    if(__user->value("f_settingsname").toString() != __c5config.fSettingsName) {
        if(__user->value("f_settingsname").toString().isEmpty() == false) {
            __c5config.fSettingsName = __user->value("f_settingsname").toString();
        }
    }

    DataDriver::init(__c5config.dbParams(), dlgsplash);

    if(!C5SystemPreference::checkDecimalPointAndSeparator()) {
        return 0;
    }

    db[":f_user"] = __user->id();
    db.exec("select sn.f_id, sn.f_name from s_settings_names sn where sn.f_id in (select f_settings from s_user_config where f_user=:f_user)");
    auto *s = new SettingsSelection();

    while(db.nextRow()) {
        s->addSettingsId(db.getInt("f_id"), db.getString("f_name"));
    }

    s->addSettingsId(-1, QObject::tr("Cancel"));

    if(db.rowCount() > 1) {
        if(s->exec() == QDialog::Accepted) {
        }
    }

    s->deleteLater();
    Working w(__user);
    w.setWindowTitle("");
    QStringList args;

    for(int i = 0; i < argc; i++) {
        args << argv[i];
    }

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

    if(__c5config.defaultHall() > 0) {
        w.setWindowTitle(w.windowTitle() + "[" + dbhall->name(__c5config.defaultHall()) + "]");
    }

    if(__c5config.defaultStore() > 0) {
        w.setWindowTitle(w.windowTitle() + "[" + dbstore->name(__c5config.defaultStore()) + "]");
    } else {
        C5Message::error(QObject::tr("Store is not defined."));
        return 0;
    }

    __c5config.setRegValue("windowtitle", w.windowTitle());
    __c5config.fParentWidget = &w;
    delete dlgsplash;
    C5Dialog::setMainWindow(&w);
    w.showMaximized();
    return a.exec();
}
