#include "threadworker.h"
#include <QException>

ThreadWorker::ThreadWorker(QObject *parent) : QObject(parent)
{
    connect(this, &ThreadWorker::endOfWork, this, &ThreadWorker::deleteLater);
}

void ThreadWorker::run()
{
    throw std::exception("ThreadWorker::run not implemented");
}
