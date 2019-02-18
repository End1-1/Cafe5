#include "dlgface.h"
#include "c5connection.h"
#include <QApplication>
#include <QTranslator>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#ifndef QT_DEBUG
    QStringList libPath;
    libPath << "./";
    libPath << "./platforms";
    libPath << "./sqldrivers";
    libPath << "./printsupport";
    QCoreApplication::setLibraryPaths(libPath);
#endif

    QList<QByteArray> connectionParams;
    C5Connection::readParams(connectionParams);
    if (connectionParams.count() > 0) {
        C5Config::fDBHost = connectionParams.at(0);
        C5Config::fDBPath = connectionParams.at(1);
        C5Config::fDBUser = connectionParams.at(2);
        C5Config::fDBPassword = connectionParams.at(3);
        C5Config::fSettingsName = connectionParams.at(4);
        C5Config::initParamsFromDb();
    } else {
        C5Dialog::go<C5Connection>(C5Config::dbParams());
    }

    QTranslator t;
    t.load(":/Waiter.qm");

    a.installTranslator(&t);
    QFont font(a.font());
    font.setFamily("Arial LatArm Unicode");
    font.setPointSize(12);
    a.setFont(font);

    DlgFace w;
    __mainWindow = &w;
    C5Config::fParentWidget = &w;
    w.showFullScreen();
    a.processEvents();
    w.setup();

    return a.exec();
}
