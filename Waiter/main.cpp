#include "dlgface.h"
#include "c5translator.h"
#include "c5connection.h"
#include "c5license.h"
#include "c5sockethandler.h"
#include <QApplication>
#include <QTranslator>
#include <QFile>
#include <QDir>
#include <QLockFile>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (!C5License::isOK()) {
        QMessageBox::critical(0, QObject::tr("Applicatin error"), QObject::tr("Please, register application."));
        return 0;
    }
#ifndef QT_DEBUG
    QStringList libPath;
    libPath << qApp->applicationDirPath();
    libPath << qApp->applicationDirPath() + "/platforms";
    libPath << qApp->applicationDirPath() + "/sqldrivers";
    libPath << qApp->applicationDirPath() + "/printsupport";
    QCoreApplication::setLibraryPaths(libPath);
#endif

    QDir d;
    QFile file(d.homePath() + "/" + _APPLICATION_ + "/lock.pid");
    file.remove();
    QLockFile lockFile(d.homePath() + "/" + _APPLICATION_ + "/lock.pid");
    if (!lockFile.tryLock()) {
        C5Message::error(QObject::tr("An instance of application already running"));
        return -1;
    }

    QList<QByteArray> connectionParams;
    C5Connection::readParams(connectionParams);
    if (connectionParams.count() > 0) {
        C5Config::fDBHost = connectionParams.at(0);
        C5Config::fDBPath = connectionParams.at(1);
        C5Config::fDBUser = connectionParams.at(2);
        C5Config::fDBPassword = connectionParams.at(3);
        C5Config::fSettingsName = connectionParams.at(4);
        C5Config::fFullScreen = connectionParams.at(6);
        C5Config::initParamsFromDb();
        C5SocketHandler::setServerAddress(C5Config::serverIP());
    } else {
        C5Dialog::go<C5Connection>(C5Config::dbParams());
    }

    QTranslator t;
    t.load(":/Waiter.qm");

    QFile styleSheet(":/waiter.qss");
    styleSheet.open(QIODevice::ReadOnly);
    a.setStyleSheet(styleSheet.readAll());

    a.installTranslator(&t);
    QFont font(a.font());
    font.setFamily("Arial LatArm Unicode");
    font.setPointSize(12);
    a.setFont(font);

    DlgFace w;
    C5Config::fParentWidget = &w;
    if (C5Config::isAppFullScreen()) {
        w.showFullScreen();
    } else {
        w.showMaximized();
    }
    a.processEvents();
    w.setup();

    return a.exec();
}
