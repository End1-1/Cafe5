#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class QMdiArea;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QMdiArea *fMdiArea;
    QPushButton *fConnectionStatusLabel;

private slots:
    void socketConnected();
    void socketDisconnected();
    void connectionButtonClicked();
    void socketDataReceived(quint16 cmd, const QByteArray &d);
    void on_actionMovement_report_triggered();

signals:
    void dataReady(QByteArray &);
};
#endif // MAINWINDOW_H
