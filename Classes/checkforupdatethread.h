#ifndef CHECKFORUPDATETHREAD_H
#define CHECKFORUPDATETHREAD_H

#include <QThread>

static const int usNone = 0;
static const int usLocalnet = 1;
static const int usInternet = 2;

class CheckForUpdateThread : public QThread
{
    Q_OBJECT

public:
    CheckForUpdateThread(QObject *parent = nullptr);

    ~CheckForUpdateThread();

    QString fApplication;

    QStringList fDbParams;

protected:
    virtual void run();

private slots:
    void startCheck();

signals:
    void go();

    void checked(bool needUpdate, int updateSource, const QString &path);
};

#endif // CHECKFORUPDATETHREAD_H
