#ifndef THREADCHECKMESSAGE_H
#define THREADCHECKMESSAGE_H

#include "threadworker.h"
#include <QSslSocket>

class ThreadCheckMessage : public ThreadWorker
{
    Q_OBJECT
public:
    ThreadCheckMessage();

protected slots:
    virtual void run() override;

    void err(QAbstractSocket::SocketError e);
};

#endif // THREADCHECKMESSAGE_H
