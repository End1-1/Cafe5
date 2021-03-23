#include "threadworker.h"
#include "logwriter.h"
#include "debug.h"
#include <QThread>

ThreadWorker::ThreadWorker() :
    QObject(nullptr)
{
    fThread = new QThread();
}

ThreadWorker::~ThreadWorker()
{
#ifdef QT_DEBUG
    qDebug() << "~ThreadWorker()";
#endif
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
    LogWriter::write(10, 1, "", QString("Error #%1: %2").arg(code).arg(errorString));
    emit finished();
}

void ThreadWorker::run()
{
    emit finished();
}
