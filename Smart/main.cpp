#include "workspace.h"
#include "c5config.h"
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
    C5Config::initParamsFromDb();

    QApplication a(argc, argv);
    QStringList dbParams;
    dbParams << C5Config::fDBHost
             << C5Config::fDBPath
             << C5Config::fDBUser
             << C5Config::fDBPassword;
    Workspace w(dbParams);
    C5Config::fParentWidget = &w;
    w.showFullScreen();
    a.processEvents();
    w.login();

    return a.exec();
}
