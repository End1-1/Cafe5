#ifndef C5SERVER5_H
#define C5SERVER5_H

#include <QWidget>
#include <QMenu>
#include <QSystemTrayIcon>

namespace Ui {
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
    void processJson(QByteArray &d);

private slots:
    void appTerminate();
    void iconClicked(QSystemTrayIcon::ActivationReason reason);
    void on_btnApply_clicked();
    void clientSocketDataRead(const QString &uuid, QByteArray &d);
    void on_btnReportsToUpload_clicked();

    void on_btnDatabase_clicked();

signals:
    void sendData(const QString &);
};

#endif // C5SERVER5_H
