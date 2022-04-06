#include "mainwindow.h"
#include "config.h"
#include "socketconnection.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString ip;
    int port;
    QString username;
    QString password;
    ServerConnection::getParams(ip, port, username, password);
    SocketConnection::init(ip, port, username, password);

    MainWindow w;
    w.show();
    return a.exec();
}
