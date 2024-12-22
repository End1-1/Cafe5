#ifndef C5SERVER5_H
#define C5SERVER5_H

#include "ninterface.h"
#include <QWidget>
#include <QMenu>
#include <QSystemTrayIcon>

namespace Ui
{
class C5Server5;
}

class WidgetContainer;
class c5scheduler;

class C5Server5 : public QWidget
{
    Q_OBJECT

public:
    explicit C5Server5(QWidget *parent = nullptr);
    ~C5Server5();

protected:
    void closeEvent(QCloseEvent *event);

private:
    bool fCanClose;
    Ui::C5Server5 *ui;
    QMenu fTrayMenu;
    QSystemTrayIcon fTrayIcon;
    WidgetContainer *wc;
    c5scheduler *scheduler;
    NInterface *fHttp;
    void printOrder(const QJsonObject &jo, QJsonArray &jcomplete);
    QDateTime fLastRunTime;
    void started();

private slots:
    void appTerminate();
    void clearBasket();
    void iconClicked(QSystemTrayIcon::ActivationReason reason);
    void on_btnApply_clicked();
    void socketConnecting();
    void socketConnected();
    void socketDisconnected();
    void timeout();
    void ordersResponse(const QJsonObject &jdoc);
    void ordersErrorResponse(const QString &err);
    void applyResponse(const QJsonObject &jdoc);
    void showNotificationWidget(const QJsonObject &jdoc);
    void notificationClosed(const QVariant &v);
    void messageReceived(const QString &message);

};

#endif // C5SERVER5_H
