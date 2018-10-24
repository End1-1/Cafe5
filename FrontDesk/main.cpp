#include "c5mainwindow.h"
#include "c5connection.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QList<QByteArray> connectionParams;
    C5Connection::readParams(connectionParams);
    C5Config::fDBHost = connectionParams.at(0);
    C5Config::fDBPath = connectionParams.at(1);
    C5Config::fDBUser = connectionParams.at(2);
    C5Config::fDBPassword = connectionParams.at(3);
    C5Config::fSettingsName = connectionParams.at(4);
    C5Config::fLastUsername = connectionParams.at(5);

    QApplication a(argc, argv);
    C5MainWindow w;
    w.showMaximized();
    a.processEvents();
    w.on_actionLogin_triggered();

    return a.exec();
}
