#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("Jazzve"));
    QCoreApplication::setApplicationName(QStringLiteral("WaiterDesigner"));

    MainWindow window;
    window.show();

    return app.exec();
}
