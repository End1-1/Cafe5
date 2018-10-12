#include "c5mainwindow.h"
#include "c5connection.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QList<QByteArray> connectionParams;
    C5Connection::readParams(connectionParams);
    C5Database::fHost = connectionParams.at(0);
    C5Database::fFile = connectionParams.at(1);
    C5Database::fUser = connectionParams.at(2);
    C5Database::fPass = connectionParams.at(3);
    C5Config::fSettingsName = connectionParams.at(4);
    C5Config::fLastUsername = connectionParams.at(5);

    QApplication a(argc, argv);
    C5MainWindow w;
    w.showMaximized();

    return a.exec();
}
