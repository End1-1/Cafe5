#include "monitor.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Monitor w;
    w.show();
    return a.exec();
}
