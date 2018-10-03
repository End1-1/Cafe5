#include <QApplication>
#include "dlgface.h"
#include "c5connection.h"

int main(int argc, char *argv[])
{
    QList<QByteArray> connectionParams;
    C5Connection::readParams(connectionParams);
    C5Database::fHost = connectionParams.at(0);
    C5Database::fFile = connectionParams.at(1);
    C5Database::fUser = connectionParams.at(2);
    C5Database::fPass = connectionParams.at(3);
    C5Config::fSettingsName = connectionParams.at(4);

    QApplication a(argc, argv);
    QFont font(a.font());
    font.setFamily("Calibri");
    font.setPointSize(12);
    a.setFont(font);
    DlgFace w;
    C5Config::fParentWidget = &w;
    w.showFullScreen();
    a.processEvents();
    w.setup();

    return a.exec();
}
