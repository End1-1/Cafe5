#include "c5threadobject.h"
#include <QThread>
#include <QDebug>

C5ThreadObject::C5ThreadObject(QObject *parent) : QObject(parent)
{

}

void C5ThreadObject::start()
{
    setParent(nullptr);
    QThread *thread = new QThread();
    connect(thread, SIGNAL(started()), this, SLOT(run()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(this, SIGNAL(finished()), thread, SLOT(quit()));
    connect(this, SIGNAL(error(int,QString)), this, SLOT(finishedWithError(int,QString)));
    moveToThread(thread);
    thread->start();
}

void C5ThreadObject::finishedWithError(int errCode, const QString &errString)
{
    emit finished();
    qDebug() << errCode << errString;
}
