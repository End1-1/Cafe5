#ifndef C5PRINTREMOVEDSERVICETHREAD_H
#define C5PRINTREMOVEDSERVICETHREAD_H

#include <QThread>
#include <QJsonObject>

class C5PrintRemovedServiceThread : public QThread
{
    Q_OBJECT

public:
    C5PrintRemovedServiceThread(const QJsonObject &h, const QJsonObject &b, QObject *parent = 0);

protected:
    virtual void run();

private:
    QJsonObject fHeader;

    QJsonObject fBody;

    void print(const QString &printName);
};

#endif // C5PRINTREMOVEDSERVICETHREAD_H
