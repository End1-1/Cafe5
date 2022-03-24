#include "c5mainwindow.h"
#include "c5connection.h"
#include "c5systempreference.h"
#include <QMessageBox>
#include <QApplication>
#include <QTranslator>
#include <QStyleFactory>
#include <QFontDatabase>
#include <ctime>

int main(int argc, char *argv[])
{

#ifndef QT_DEBUG
    QStringList libPath = QCoreApplication::libraryPaths();
    libPath << qApp->applicationDirPath();
    libPath << qApp->applicationDirPath() + "/platforms";
    libPath << qApp->applicationDirPath() + "/sqldrivers";
    libPath << qApp->applicationDirPath() + "/printsupport";
    libPath << qApp->applicationDirPath() + "/imageformats";
    QCoreApplication::setLibraryPaths(libPath);
#endif

    QApplication a(argc, argv);

    if (!C5SystemPreference::checkDecimalPointAndSeparator()) {
        return 0;
    }

    for (int i = 1; i < argc; i++){
        __autologin_store.append(argv[i]);
    }

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

    QTranslator t;
    t.load(":/lang/FrontDesk.qm");
    a.installTranslator(&t);
    a.setStyle(QStyleFactory::create("fusion"));

    QFontDatabase::addApplicationFont(":/barcode.ttf");
//    int id = QFontDatabase::addApplicationFont(":/ahuni.ttf");
//    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont font(a.font());
    font.setPointSize(C5Config::fronDeskFontSize());
    a.setFont(font);

    srand(time(NULL));
    C5MainWindow w;
    w.showMaximized();
    a.processEvents();
    w.on_actionLogin_triggered();

    return a.exec();
}
