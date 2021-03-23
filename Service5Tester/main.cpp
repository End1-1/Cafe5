#include "testerdialog.h"

#include <QApplication>
#include <QSslSocket>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << QSslSocket::supportsSsl();
    qDebug() << QLocale().toString(1000000);

    TesterDialog w;
    w.show();
    return a.exec();
}
