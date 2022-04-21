#include "thread.h"
#include <QDebug>

Thread::Thread(const QString &n) :
    QThread(nullptr),
    fName(n)
{

}

Thread::~Thread()
{
    qDebug() << "~Thread()" << fName;
}

void Thread::quit()
{
    QThread::quit();
}

Thread::Thread(QObject *parent) :
    QThread(parent)
{

}
