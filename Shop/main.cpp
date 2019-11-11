#include "working.h"
#include "c5config.h"
#include "c5connection.h"
#include <QApplication>
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

    __userid = 1;
    Working w;
    w.showMaximized();

    return a.exec();
}
