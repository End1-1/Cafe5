#ifndef THREADWORKER_H
#define THREADWORKER_H

#include <QObject>

class ThreadWorker : public QObject
{
    Q_OBJECT
public:
    explicit ThreadWorker();
    ~ThreadWorker();
    void start();

public slots:
    virtual void run();

signals:
    void data(int code, const QVariant &emittedData);
    void done();
    void threadError(int,QString);

};

#endif // THREADWORKER_H
