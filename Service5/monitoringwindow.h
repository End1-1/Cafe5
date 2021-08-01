#ifndef MONITORINGWINDOW_H
#define MONITORINGWINDOW_H

#include <QObject>

class MonitoringWindow : public QObject
{
    Q_OBJECT
public:    
    static void init();

    static void connectReceiver(QObject *r);

    static void connectSender(QObject *s);

public slots:
    void monitoring(int code, const QString &session, const QString &data1, const QVariant &data2);

private:
    explicit MonitoringWindow(QObject *parent = nullptr);

    static MonitoringWindow *fInstance;

signals:
    void translateData(int code, const QString &, const QString &, const QVariant &);

};

#endif // MONITORINGWINDOW_H
