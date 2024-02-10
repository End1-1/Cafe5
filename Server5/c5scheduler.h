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

private:
    QTimer fTimer;
    bool fRun;

    void runServicePrint();

};

#endif // C5SCHEDULER_H
