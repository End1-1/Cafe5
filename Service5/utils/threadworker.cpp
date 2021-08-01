#include "threadworker.h"
#include "logwriter.h"
#include "debug.h"
#include <QThread>

ThreadWorker::ThreadWorker() :
    QObject(nullptr)
{

}

ThreadWorker::~ThreadWorker()
{
    qDebug() << "~ThreadWorker()";
}

void ThreadWorker::start()
{
    run();
}

void ThreadWorker::run()
{
    qDebug() << "JUST RUN";
    emit done();
}
