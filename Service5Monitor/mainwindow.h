#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QMdiArea;
class SocketConnection;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void socketConnected();
    void socketDisconnected();
    void on_actionOpenMap_triggered();

    void on_actionCommon_statistics_triggered();

private:
    Ui::MainWindow *ui;
    QMdiArea *fMdiArea;
    QLabel *fConnectionStatusLabel;
};
#endif // MAINWINDOW_H
