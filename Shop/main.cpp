#include "working.h"
#include "c5config.h"
#include "c5license.h"
#include "dlgpin.h"
#include "c5logsystem.h"
#include "datadriver.h"
#include "c5user.h"
#include "c5servername.h"
#include "settingsselection.h"
#include "c5systempreference.h"
#include "dlgsplashscreen.h"
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
#include <QSettings>
int main(int argc, char *argv[])
{
#ifndef QT_DEBUG
    QStringList libPath;
    libPath << "./";
    libPath << "./platforms";
    libPath << "./sqldrivers";
    libPath << "./printsupport";
    QCoreApplication::setLibraryPaths(libPath);
#endif

    QDir d;
    QLockFile lockFile(d.homePath() + "/" + _APPLICATION_ + "/lock.pid");
    if (!lockFile.tryLock()) {
        C5Message::error(QObject::tr("An instance of application already running"));
        return -1;
    }

    QApplication a(argc, argv);

    QTranslator t;
    t.load(":/lang/Shop.qm");
    a.installTranslator(&t);

    auto *dlgsplash = new DlgSplashScreen();
    dlgsplash->show();
    dlgsplash->messageSignal("Get server name");

    QString settingsName;
    bool noconfig = true;
    for (const QString &arg: a.arguments()) {
        if (arg.contains("/servername=")){
            QStringList sn = arg.split("=", Qt::SkipEmptyParts);
            if (sn.length() == 2) {
                C5ServerName sng(sn.at(1), "shop");
                if (!sng.getServers()){
                    return 1;
                }
                noconfig = false;
                QJsonObject js = sng.mServers.at(0).toObject();

                C5Config::fDBHost = js["host"].toString();
                C5Config::fDBPath = js["database"].toString();
                C5Config::fDBUser = js["username"].toString();
                C5Config::fDBPassword = js["password"].toString();
                C5Config::fSettingsName = js["settings"].toString();
                QSettings ss(_ORGANIZATION_, _APPLICATION_+ QString("\\") + _MODULE_);
                ss.setValue("server", "");
            }
        } else if (arg.contains("/config")) {
            QStringList sn = arg.split("=", Qt::SkipEmptyParts);
            if (sn.length() == 2) {
                settingsName = sn.at(1);
            }
        }
    }
    if (!settingsName.isEmpty()) {
        C5Config::fSettingsName = settingsName;
    }
    if (noconfig) {
        QMessageBox::critical(0, "Error", "No config");
        return 1;
    }

    if (!d.exists(d.homePath() + "/" + _APPLICATION_)) {
        d.mkpath(d.homePath() + "/" + _APPLICATION_);
    }
    if (!d.exists(d.homePath() + "/" + _APPLICATION_ + "/logs")) {
        d.mkpath(d.homePath() + "/" + _APPLICATION_ + "/logs");
    }
    ls(QObject::tr("Application start"));

    a.setStyle(QStyleFactory::create("fusion"));
    QFile styleSheet("./styles.qss");
    if (styleSheet.exists()) {
        styleSheet.open(QIODevice::ReadOnly);
        a.setStyleSheet(styleSheet.readAll());
    }

    QStringList args;
    for (int i = 0; i < argc; i++) {
        args << argv[i];
    }

    C5Config::initParamsFromDb();
    dlgsplash->updateData();

    if (!C5SystemPreference::checkDecimalPointAndSeparator()) {
        return 0;
    }

    QFontDatabase::addApplicationFont(":/barcode.ttf");
//    int id = QFontDatabase::addApplicationFont(":/ahuni.ttf");
//    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFile shopstyle(a.applicationDirPath() + "/shopstyle.qss");
    if (shopstyle.exists()) {
        if (shopstyle.open(QIODevice::ReadOnly)) {
            a.setStyleSheet(shopstyle.readAll());
        }
    }

    C5Database db(C5Config::dbParams());
    QString user, pin;
    __user = new C5User(0);
    if (!__c5config.shopEnterPin()) {
        user = __c5config.getValue(param_shop_autologin_pin1);
        pin = __c5config.getValue(param_shop_autologin_pin2);
        __user->authByPinPass(user, pin);
    }
    while (!__user->isValid() || !__user->isActive()) {
        if (!DlgPin::getPin(user, pin)) {
            return 0;
        }

        if (__user->authByPinPass(user, pin)) {

        } else {
            C5Message::error(__user->error());
        }
        pin.clear();
        user.clear();
    };

    db[":f_user"] = __user->id();
    db.exec("select sn.f_id, sn.f_name from s_settings_names sn where sn.f_id in (select f_settings from s_user_config where f_user=:f_user)");
    auto *s = new SettingsSelection();
    while (db.nextRow()) {
        s->addSettingsId(db.getInt("f_id"), db.getString("f_name"));
    }
    s->addSettingsId(-1, QObject::tr("Cancel"));
    if (db.rowCount() > 1) {
        if (s->exec() == QDialog::Accepted) {

        }
    }
    s->deleteLater();

    Working w(__user);
    w.setWindowTitle("");
    if (__c5config.defaultHall() > 0) {
        w.setWindowTitle(w.windowTitle() + "[" + dbhall->name(__c5config.defaultHall()) + "]");
    }
    if (__c5config.defaultStore() > 0) {
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
