#ifndef C5SCHEDULER_H
#define C5SCHEDULER_H

#include <QObject>
#include <QTimer>

class QNetworkReply;

class c5scheduler : public QObject
{
    Q_OBJECT
public:
    explicit c5scheduler(QObject *parent = nullptr);

signals:

public slots:
    void timeout();
    void replyFinished(QNetworkReply *r);

private:
    QTimer fTimer;
    bool fRun;
};

#endif // C5SCHEDULER_H
