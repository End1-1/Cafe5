#include "thread.h"
#include <QDebug>

Thread::Thread()
{

}

Thread::~Thread()
{
    qDebug() << "~Thread()";
}
