#include "testerdialog.h"

#include <QApplication>
#include <QSslSocket>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TesterDialog w;
    w.show();
    return a.exec();
}
