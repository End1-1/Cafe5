#include "workspace.h"
#include "c5config.h"
#include "c5systempreference.h"
#include "datadriver.h"
#include <QFile>
#include <QLockFile>
#include "c5connection.h"
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
    C5Config::fFullScreen = true;
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
