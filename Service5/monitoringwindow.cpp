#include "monitoringwindow.h"

MonitoringWindow *MonitoringWindow::fInstance = nullptr;

void MonitoringWindow::init()
{
    fInstance = new MonitoringWindow();
}

void MonitoringWindow::connectReceiver(QObject *r)
{
    r->connect(fInstance, SIGNAL(translateData(int,QString,QString,QVariant)), r, SLOT(receiveData(int,QString,QString,QVariant)));
}

void MonitoringWindow::connectSender(QObject *s)
{
    s->connect(s, SIGNAL(sendData(int,QString,QString,QVariant)), fInstance, SLOT(monitoring(int,QString,QString,QVariant)));
}

void MonitoringWindow::monitoring(int code, const QString &session, const QString &data1, const QVariant &data2)
{
    emit translateData(code, session, data1, data2);
}

MonitoringWindow::MonitoringWindow(QObject *parent) :
    QObject(parent)
{

}
