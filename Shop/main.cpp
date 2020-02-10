#include "working.h"
#include "c5config.h"
#include "c5connection.h"
#include "c5license.h"
#include "dlgpin.h"
#include <QApplication>
#include <QMessageBox>
#include <QTranslator>

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
    if (!C5License::isOK()) {
        QMessageBox::critical(0, QObject::tr("Application error"), QObject::tr("Please, register application."));
        return 0;
    }

    QTranslator t;
    t.load(":/lang/Shop.qm");
    a.installTranslator(&t);

    QList<QByteArray> connectionParams;
    C5Connection::readParams(connectionParams);
    C5Config::fDBHost = connectionParams.at(0);
    C5Config::fDBPath = connectionParams.at(1);
    C5Config::fDBUser = connectionParams.at(2);
    C5Config::fDBPassword = connectionParams.at(3);
    C5Config::fSettingsName = connectionParams.at(4);
    C5Config::initParamsFromDb();
    C5Database::uuid(C5Config::dbParams());

    if (C5Config::shopEnterPin()) {
        bool login = false;
        do {
            QString user, pin;
            if (!DlgPin::getPin(user, pin)) {
                return 0;
            }
            C5Database db(C5Config::dbParams());
            db[":f_login"] = user;
            db[":f_password"] = password(pin);
            db.exec("select f_id, f_group, f_state, concat(f_last, ' ', f_first) from s_user where f_login=:f_login and f_altpassword=:f_password");
            if (!db.nextRow()) {
                C5Message::error(QObject::tr("Login failed"));
            } else if (db.getInt(2) == 0) {
                C5Message::error(QObject::tr("User is inactive"));
            } else {
                __userid = db.getInt(0);
                __usergroup = db.getInt(1);
                __username = db.getString(3);
                login = true;
            }
        } while (!login);
    } else {
        __userid = 1;
    }
    Working w;
    w.showMaximized();

    return a.exec();
}
