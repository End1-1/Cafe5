#include "dashboard.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dashboard w;
    __dashboard = &w;
    w.showMaximized();
    a.processEvents();
    w.on_btnLogin_clicked();

    return a.exec();
}
