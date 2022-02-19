#include "workspace.h"
#include "c5config.h"
#include "c5systempreference.h"
#include <QFile>
#include <QLockFile>
#include "c5connection.h"
#include <QApplication>
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

    if (!C5SystemPreference::checkDecimalPointAndSeparator()) {
        return 0;
    }

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
    if (args.contains("--config")) {
        C5Connection *c = new C5Connection(QStringList());
        c->exec();
        delete c;
    }
    QFont f("Arial LatArm Unicode", 10);
    qApp->setFont(f);
    QList<QByteArray> connectionParams;
    C5Connection::readParams(connectionParams);
    C5Config::fDBHost = connectionParams.at(0);
    C5Config::fDBPath = connectionParams.at(1);
    C5Config::fDBUser = connectionParams.at(2);
    C5Config::fDBPassword = connectionParams.at(3);
    C5Config::fSettingsName = connectionParams.at(4);
    C5Config::fLastUsername = connectionParams.at(5);
    C5Config::fFullScreen = connectionParams.at(6);
    C5Config::initParamsFromDb();

    QStringList dbParams;
    dbParams << C5Config::fDBHost
             << C5Config::fDBPath
             << C5Config::fDBUser
             << C5Config::fDBPassword;
    Workspace w(dbParams);
    C5Config::fParentWidget = &w;
    w.showFullScreen();
    a.processEvents();
    if (!w.login()) {
        return 0;
    }

    return a.exec();
}
