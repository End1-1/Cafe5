#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include "threadworker.h"

class SslServer;

class ServerThread : public ThreadWorker
{
    Q_OBJECT
public:
    ServerThread(const QString &configPath);

protected slots:
    virtual void run() override;

private:
    SslServer *fSslServer;
    const QString fConfigPath;
};

#endif // SERVERTHREAD_H
