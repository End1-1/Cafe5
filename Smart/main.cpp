#include "workspace.h"
#include "c5config.h"
#include "c5connection.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
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
    QFont f("Arial", 10);
    qApp->setFont(f);
    QList<QByteArray> connectionParams;
    C5Connection::readParams(connectionParams);
    C5Config::fDBHost = connectionParams.at(0);
    C5Config::fDBPath = connectionParams.at(1);
    C5Config::fDBUser = connectionParams.at(2);
    C5Config::fDBPassword = connectionParams.at(3);
    C5Config::fSettingsName = connectionParams.at(4);
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
    w.login();

    return a.exec();
}
