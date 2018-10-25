#include "dlgface.h"
#include "c5connection.h"
#include <QApplication>
#include <QTranslator>

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

    QTranslator t;
    t.load(":/Waiter.qm");

    QApplication a(argc, argv);
    a.installTranslator(&t);
    QFont font(a.font());
    font.setFamily("Arial LatArm Unicode");
    font.setPointSize(12);
    a.setFont(font);
    DlgFace w;
    C5Config::fParentWidget = &w;
    w.showFullScreen();
    a.processEvents();
    w.setup();

    return a.exec();
}
