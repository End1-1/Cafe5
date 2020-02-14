#ifndef C5THREADOBJECT_H
#define C5THREADOBJECT_H

#include <QObject>
#include <QWidget>

class C5ThreadObject : public QObject
{
    Q_OBJECT
public:
    explicit C5ThreadObject(QObject *parent = nullptr);
    void start();

protected slots:
    virtual void run() = 0;
    virtual void finishedWithError(int errCode, const QString &errString);

signals:
    void finished();
    void finishThread();
    void error(int errCode, const QString &errString);

};

#endif // C5THREADOBJECT_H
