#include "threadworker.h"
#include <QException>

ThreadWorker::ThreadWorker(QObject *parent) : QObject(parent)
{
    connect(this, &ThreadWorker::endOfWork, this, &ThreadWorker::deleteLater);
}

ThreadWorker::~ThreadWorker()
{

}

void ThreadWorker::run()
{
    throw std::exception("ThreadWorker::run not implemented");
}
