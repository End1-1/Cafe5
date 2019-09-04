#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include "widgetcontrols.h"
#include <QWidget>
#include <QMenu>
#include <QSystemTrayIcon>

namespace Ui {
class ServerWindow;
}

class ServerWindow : public QWidget, WidgetControls
{
    Q_OBJECT

public:
    explicit ServerWindow(QWidget *parent = nullptr);
    ~ServerWindow();
    virtual void setWidgetContainer() override;

protected:
    virtual void closeEvent(QCloseEvent *e) override;

private:
    Ui::ServerWindow *ui;
    QMenu fTrayMenu;
    QSystemTrayIcon fTrayIcon;
    bool fCanClose;
    QMap<QString, int> fUUIDRow;
    void applyConfiguration();

private slots:
    void registerConnection(const QString &uuid, const QString &inout, const QString &remoteIp, const QString &host, const QString &user);
    void unregisterConnection(const QString &uuid, const QString &reason);
    void connectionRXTX(const QString &uuid, int rx, int tx);
    void appTerminate();
    void iconClicked(QSystemTrayIcon::ActivationReason reason);
    void on_btnSettings_clicked();
    void on_btnSaveSettings_clicked();
    void on_btnConnections_clicked();
};

#endif // SERVERWINDOW_H
