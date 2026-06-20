#include <QApplication>

#include "dlgfiscaltest.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("Jazzve"));
    QCoreApplication::setApplicationName(QStringLiteral("TestFiscal"));

    DlgFiscalTest dlg;
    dlg.show();
    dlg.raise();
    dlg.activateWindow();

    return app.exec();
}
