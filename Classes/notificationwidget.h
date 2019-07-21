#ifndef NOTIFICATIONWIDGET_H
#define NOTIFICATIONWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QThread>

namespace Ui {
class NotificationWidget;
}

class NotificationThread : public QObject {

    Q_OBJECT

public:
    NotificationThread(const QString &message, int info);

private:
    QString fMessage;
    int fInfo;

private slots:
    void run();

signals:
    void go();
};

class NotificationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NotificationWidget(const QString &message, int info, QWidget *parent = nullptr);
    ~NotificationWidget();
    static void showMessage(const QString &message, int info = 0);
    static void compact();
    static QList<NotificationWidget*> fWidgetList;
    QTimer fTimer;

private slots:
    void timeout();
    void on_btnClose_clicked();

private:
    Ui::NotificationWidget *ui;
    bool fClose;
};

#endif // NOTIFICATIONWIDGET_H
