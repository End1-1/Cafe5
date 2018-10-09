#ifndef C5PRINTSERVICETHREAD_H
#define C5PRINTSERVICETHREAD_H

#include <QThread>
#include <QJsonArray>
#include <QJsonObject>

class C5PrintServiceThread : public QThread
{
public:
    C5PrintServiceThread(const QJsonObject &header, const QJsonArray &body, QObject *parent = 0);

    ~C5PrintServiceThread();

protected:
    virtual void run();

private:
    QJsonObject fHeader;

    QJsonArray fBody;

    void print(const QString &printer, const QString &side);
};

#endif // C5PRINTSERVICETHREAD_H
