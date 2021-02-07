#include "monitor.h"
#include "ui_monitor.h"
#include "serverthread.h"

Monitor::Monitor(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Monitor)
{
    ui->setupUi(this);
    startSslServer();

}

Monitor::~Monitor()
{
    delete ui;
}

void Monitor::startSslServer()
{
    fSslServer = new ServerThread(qApp->applicationDirPath() + "/");
    fSslServer->start();
}

