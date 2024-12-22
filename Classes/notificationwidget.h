#ifndef NOTIFICATIONWIDGET_H
#define NOTIFICATIONWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QVariant>

namespace Ui
{
class NotificationWidget;
}

class NotificationThread : public QObject
{

    Q_OBJECT

public:
    NotificationThread(const QString &message, const QVariant &callbackData, char *callback, QObject *callbackObject,
                       int info);
    bool fClose;
    char *fCallback;
    QVariant fCallbackData;
    QObject *fCallbackObject;
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
    static void showMessage(const QString &message, const QVariant &callBackData, char *callBack, QObject *callbackObject, int info = 0);
    static void compact();
    static QList<NotificationWidget *> fWidgetList;
    QTimer fTimer;
    char *fCallback;
    QVariant fCallbackData;
    QObject *fCallbackObject;
private slots:
    void timeout();
    void on_btnClose_clicked();

private:
    Ui::NotificationWidget *ui;
    bool fClose;

signals:
    void onClose(const QVariant &);
};

#endif // NOTIFICATIONWIDGET_H
