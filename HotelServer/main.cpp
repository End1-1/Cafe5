#include "serverwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ServerWindow w;
    w.show();
    w.setWidgetContainer();
    //w.hide();

    return a.exec();
}
