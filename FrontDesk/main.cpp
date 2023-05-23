#include "c5mainwindow.h"
#include "c5connection.h"
#include "c5systempreference.h"
#include <QMessageBox>
#include <QApplication>
#include <QTranslator>
#include <QStyleFactory>
#include <QFontDatabase>
#include <QFile>
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

    for (int i = 1; i < argc; i++){
        __autologin_store.append(argv[i]);
    }

//    if (QDate::currentDate() > QDate::fromString("20/05/2023", "dd/MM/yyyy")) {
//        return 1;
//    }

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

    if (!C5SystemPreference::checkDecimalPointAndSeparator()) {
        return 0;
    }

    QTranslator t;
    t.load(":/lang/FrontDesk.qm");
    a.installTranslator(&t);
    a.setStyle(QStyleFactory::create("fusion"));

    QFontDatabase::addApplicationFont(":/barcode.ttf");
//    int id = QFontDatabase::addApplicationFont(":/ahuni.ttf");
//    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont font(a.font());
    font.setPointSize(__c5config.fronDeskFontSize());
    font.setFamily(__c5config.getValue(param_app_font_family));
    a.setFont(font);

    QFile style(a.applicationDirPath() + "/officestyle.qss");
    if (style.exists()) {
        if (style.open(QIODevice::ReadOnly)) {
            QString s = style.readAll();
            s.replace("%font-size%", __c5config.getValue(param_fd_font_size));
            s.replace("%font-family%", __c5config.getValue(param_app_font_family));
            a.setStyleSheet(s);
        }
    }

    srand(time(NULL));
    C5MainWindow w;
    w.showMaximized();
    a.processEvents();
    w.on_actionLogin_triggered();

    return a.exec();
}
