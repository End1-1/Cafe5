#ifndef THREADWORKER_H
#define THREADWORKER_H

#include <QObject>
#include <QThread>

class ThreadWorker : public QObject
{
    Q_OBJECT
public:
    explicit ThreadWorker();
    ~ThreadWorker();
    void start();

public slots:
    void finishedWithError(int code, const QString &errorString);

protected:
    QThread *fThread;

protected slots:
    virtual void run();

signals:
    void data(int code, const QVariant &emittedData);
    void finished();
    void threadError(int , const QString &errorString);

};

#endif // THREADWORKER_H
