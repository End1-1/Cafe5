#ifndef C5MAINWINDOW_H
#define C5MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class C5MainWindow;
}

class C5MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit C5MainWindow(QWidget *parent = 0);
    ~C5MainWindow();
    virtual void closeEvent(QCloseEvent *event);
private slots:
    void on_action_triggered();

    void on_actionConnection_triggered();

private:
    Ui::C5MainWindow *ui;
};

#endif // C5MAINWINDOW_H
