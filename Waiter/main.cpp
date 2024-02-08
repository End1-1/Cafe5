#include "c5translator.h"
#include "c5connection.h"
#include "c5sockethandler.h"
#include "datadriver.h"
#include "dlgscreen.h"
#include "c5menu.h"
#include "c5systempreference.h"
#include <QApplication>
#include <QTranslator>
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

//    if (QDate::currentDate() > QDate::fromString("01/10/2022", "dd/MM/yyyy")) {
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

    QStringList args;
    for (int i = 0; i < argc; i++) {
        args << argv[i];
    }
    QSettings ss(_ORGANIZATION_, _APPLICATION_+ QString("\\") + _MODULE_);
    QJsonObject js = QJsonDocument::fromJson(ss.value("server", "{}").toByteArray()).object();
    if (args.contains("--config")) {
        C5Connection c(js);
        if (c.exec() == QDialog::Accepted) {
            js = c.fParams;
            ss.setValue("server", QJsonDocument(js).toJson(QJsonDocument::Compact));
        }
    }
    C5Config::fDBHost = js["host"].toString();
    C5Config::fDBPath = js["database"].toString();
    C5Config::fDBUser = js["username"].toString();
    C5Config::fDBPassword = js["password"].toString();
    C5Config::fSettingsName = js["settings"].toString();
    C5Config::fFullScreen = js["fullscreen"].toBool();
    C5SocketHandler::setServerAddress(js["waiter_server"].toString());
    C5Config::initParamsFromDb();

    C5Database::LOGGING = C5Config::getValue(param_debuge_mode).toInt() == 1;

    if (!C5SystemPreference::checkDecimalPointAndSeparator()) {
        return 0;
    }

    QTranslator t;
    t.load(":/Waiter.qm");

    QFile styleSheet(a.applicationDirPath() + "/waiter.qss");
    if (styleSheet.open(QIODevice::ReadOnly)) {
        a.setStyleSheet(styleSheet.readAll());
        styleSheet.close();
    }

    a.installTranslator(&t);
    QFont font(a.font());
    font.setFamily("Arial LatArm Unicode");
    font.setPointSize(11);
    a.setFont(font);

    //C5Message::info("{seq:1,paidAmount:0, paidAmountCard:1980, partialAmount:0, prePaymentAmount:0, useExtPOS:true, mode:2,  items:[{adgCode:56.10,dep:1,price:1800,productCode:1429,productName:DARGETT STOUT ,qty:1,totalPrice:1800,unit:հատ},{adgCode:5901,dep:1,price:180,productCode:001,productName:Սպասարկում 10%,qty:1,totalPrice:180,unit:հատ}]}");

    C5Database db(__c5config.dbParams());
    if (db.open()) {
        DbData::setDBParams(__c5config.dbParams());
        DataDriver::init(__c5config.dbParams());
        menu5 = new C5Menu();
        menu5->refresh();
    } else {
        C5Message::error(db.fLastError);
    }

    DlgScreen w;
    C5Config::fParentWidget = &w;
    if (C5Config::isAppFullScreen()) {
        w.setWindowState(Qt::WindowFullScreen);
    }
    w.show();
    a.processEvents();

    return a.exec();
}
