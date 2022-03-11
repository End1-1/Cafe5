#ifndef THREAD_H
#define THREAD_H

#include <QThread>

class Thread : public QThread
{
    Q_OBJECT
public:
    Thread(const QString &n);
    ~Thread();

public Q_SLOTS:
    void quit();

private:
    Thread(QObject *parent = nullptr);
    QString fName;
};

#endif // THREAD_H
