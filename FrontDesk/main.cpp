#include "c5mainwindow.h"
#include "c5connection.h"
#include "c5cache.h"
#include <QApplication>
#include <QTranslator>
#include <QStyleFactory>
#include <QFontDatabase>

int main(int argc, char *argv[])
{

#ifndef QT_DEBUG
    QStringList libPath = QCoreApplication::libraryPaths();
    libPath << "./";
    libPath << "./platforms";
    libPath << "./sqldrivers";
    libPath << "./printsupport";
    QCoreApplication::setLibraryPaths(libPath);
#endif

    QApplication a(argc, argv);

    QList<QByteArray> connectionParams;
    C5Connection::readParams(connectionParams);
    C5Config::fDBHost = connectionParams.at(0);
    C5Config::fDBPath = connectionParams.at(1);
    C5Config::fDBUser = connectionParams.at(2);
    C5Config::fDBPassword = connectionParams.at(3);
    C5Config::fSettingsName = connectionParams.at(4);
    C5Config::fLastUsername = connectionParams.at(5);
    C5Config::initParamsFromDb();

    QTranslator t;
    t.load(":/lang/FrontDesk.qm");
    a.installTranslator(&t);
    a.setStyle(QStyleFactory::create("fusion"));

    int id = QFontDatabase::addApplicationFont(":/ahuni.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont font(family);
    font.setPointSize(8);
    a.setFont(font);

    C5MainWindow w;
    w.showMaximized();
    a.processEvents();
    w.on_actionLogin_triggered();

    return a.exec();
}
