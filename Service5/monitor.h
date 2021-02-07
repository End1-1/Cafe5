#ifndef MONITOR_H
#define MONITOR_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class Monitor; }
QT_END_NAMESPACE

class ServerThread;

class Monitor : public QDialog
{
    Q_OBJECT

public:
    Monitor(QWidget *parent = nullptr);
    ~Monitor();

private:
    Ui::Monitor *ui;
    ServerThread *fSslServer;
    void startSslServer();
};
#endif // MONITOR_H
