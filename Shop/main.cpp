#include "working.h"
#include "c5config.h"
#include "c5connection.h"
#include "c5license.h"
#include "dlgpin.h"
#include "c5logsystem.h"
#include "c5user.h"
#include "c5replication.h"
#include "datadriver.h"
#include "replicadialog.h"
#include "settingsselection.h"
#include "dlgsplashscreen.h"
#include <QApplication>
#include <QMessageBox>
#include <QLockFile>
#include <QInputDialog>
#include <QDir>
#include <QTranslator>
#include <QStyleFactory>

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


    QApplication a(argc, argv);
    QDir d;
    if (!d.exists(d.homePath() + "/" + _APPLICATION_)) {
        d.mkpath(d.homePath() + "/" + _APPLICATION_);
    }
    if (!d.exists(d.homePath() + "/" + _APPLICATION_ + "/logs")) {
        d.mkpath(d.homePath() + "/" + _APPLICATION_ + "/logs");
    }
    ls(QObject::tr("Application start"));
    QString err;
    if (!C5License::isOK(err)) {
        ls(QObject::tr("License check failed"));
        QMessageBox::critical(0, QObject::tr("Application error"), err);
        return 0;
    }

    a.setStyle(QStyleFactory::create("fusion"));
    QFile styleSheet("./styles.qss");
    if (styleSheet.exists()) {
        styleSheet.open(QIODevice::ReadOnly);
        a.setStyleSheet(styleSheet.readAll());
    }

    QTranslator t;
    t.load(":/lang/Shop.qm");
    a.installTranslator(&t);

    QStringList args;
    for (int i = 0; i < argc; i++) {
        args << argv[i];
    }
    if (args.contains("--config")) {
        QList<QByteArray> connectionParams;
        C5Connection::readParams(connectionParams);
        if (connectionParams.count() > 3) {
            if (connectionParams[3].length() > 0) {
                QString password = QInputDialog::getText(nullptr, QObject::tr("Password"), QObject::tr("Password"), QLineEdit::Password);
                if (connectionParams[3] != password) {
                    C5Message::error(QObject::tr("Access denied"));
                    return 0;
                }
            }
        }
        const QStringList dbParams;
        C5Connection *cnf = new C5Connection(dbParams);
        cnf->exec();
        delete cnf;
    }

    QList<QByteArray> connectionParams;
    C5Connection::readParams(connectionParams);
    C5Config::fDBHost = connectionParams.at(0);
    C5Config::fDBPath = connectionParams.at(1);
    C5Config::fDBUser = connectionParams.at(2);
    C5Config::fDBPassword = connectionParams.at(3);
    C5Config::fSettingsName = connectionParams.at(4);
    C5Config::initParamsFromDb();

    QFile file(d.homePath() + "/" + _APPLICATION_ + "/lock.pid");
    file.remove();
    QLockFile lockFile(d.homePath() + "/" + _APPLICATION_ + "/lock.pid");
    if (!lockFile.tryLock()) {
        ls(QObject::tr("Application instance found, exiting"));
        C5Message::error(QObject::tr("An instance of application already running"));
        return -1;
    }

    bool login = false;
    C5Database db(C5Config::dbParams());
    QString user, pin;
    C5User ua(0);
    if (!__c5config.shopEnterPin()) {
        user = __c5config.getValue(param_shop_autologin_pin1);
        pin = __c5config.getValue(param_shop_autologin_pin2);
    }
    do {
        if (!DlgPin::getPin(user, pin)) {
            return 0;
        }

        if (ua.authByPinPass(user, pin)) {
            login = true;
        } else {
            C5Message::error(ua.error());
        }
        pin.clear();
        user.clear();
    } while (!login);
    db[":f_group"] = ua.group();
    db.exec("select f_key from s_user_access where f_group=:f_group");
    while (db.nextRow()) {
        __userpermissions.append(db.getInt(0));
    }
    db[":f_user"] = ua.id();
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


    if (__c5config.rdbReplica()) {
        auto *r = new C5Replication();
        r->uploadToServer();
        delete r;

        auto *rp = new ReplicaDialog();
        rp->exec();
        delete rp;
        __c5config.initParamsFromDb();
    }
    __userid = ua.id();
    __usergroup = ua.group();

    DataDriver::init(__c5config.dbParams());
    C5Permissions::init(db);

    auto *dlgsplash = new DlgSplashScreen();
    dlgsplash->exec();
    delete dlgsplash;

    Working w;
    w.setWindowTitle(dbhall->name(__c5config.defaultHall()) + "," + dbstore->name(__c5config.defaultStore()));
    __c5config.fParentWidget = &w;
    C5Dialog::setMainWindow(&w);
    w.showMaximized();

    return a.exec();
}
