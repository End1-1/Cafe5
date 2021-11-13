#ifndef THREADWORKER_H
#define THREADWORKER_H

#include <QObject>

class ThreadWorker : public QObject
{
    Q_OBJECT
public:
    explicit ThreadWorker(QObject *parent = nullptr);

public slots:
    virtual void run();

signals:
    void endOfWork();
    void sendData(int code, const QString &, const QString &, const QVariant &);
};

#endif // THREADWORKER_H
