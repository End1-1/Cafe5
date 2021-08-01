#ifndef THREADREADMESSAGE_H
#define THREADREADMESSAGE_H

#include "threadworker.h"
#include <QSslSocket>

class ThreadReadMessage : public ThreadWorker
{
    Q_OBJECT
public:
    ThreadReadMessage(const QString &list);

protected slots:
    void run();

private:
    QString fList;

private slots:
    void err(QAbstractSocket::SocketError e);
};

#endif // THREADREADMESSAGE_H
