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
    QPushButton *fConnectionStatusLabel;
    QPushButton *fLoginButton;
    void addTabWidget(QWidget *w, const QIcon &icon, const QString &title);

private slots:
    void socketConnected();
    void socketDisconnected();
    void connectionButtonClicked();
    void loginButtonClicked();
    void socketDataReceived(quint16 cmd, quint32 messageId, QByteArray d);
    void on_actionMovement_report_triggered();
    void on_tw_tabCloseRequested(int index);
    void on_tw_currentChanged(int index);

signals:
    void dataReady(const QByteArray &);
};
#endif // MAINWINDOW_H
