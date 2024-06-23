#include "threadworker.h"
#include <QThread>

ThreadWorker::ThreadWorker() :
    QObject(nullptr)
{
    fThread = new QThread();
}

ThreadWorker::~ThreadWorker()
{
}

void ThreadWorker::start()
{
    connect(fThread, SIGNAL(started()), this, SLOT(run()));
    connect(fThread, SIGNAL(finished()), fThread, SLOT(deleteLater()));
    connect(this, SIGNAL(finished()), fThread, SLOT(quit()));
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    connect(this, SIGNAL(threadError(int, QString)), this, SLOT(finishedWithError(int, QString)));
    moveToThread(fThread);
    fThread->start();
}

void ThreadWorker::finishedWithError(int code, const QString &errorString)
{
    Q_UNUSED(code);
    Q_UNUSED(errorString);
    emit finished();
}

void ThreadWorker::run()
{
    emit finished();
}
