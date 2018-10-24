#ifndef C5MAINWINDOW_H
#define C5MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTreeWidgetItem>

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

    template<typename T>
    T *createTab(const QStringList &dbParams) {
        T *t = new T(dbParams);
        fTab->addTab(t, t->icon(), QString("[%1] %2").arg(dbParams.at(4)).arg(t->label()));
        fTab->setCurrentIndex(fTab->count() - 1);
        t->postProcess();
        return t;
    }

public slots:
    void on_actionLogin_triggered();

private slots:
    void hotKey();

    void twCustomMenu(const QPoint &p);

    void tabCloseRequested(int index);

    void currentTabChange(int index);

    void on_actionConnection_triggered();

    void on_twDb_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_actionConfigure_connection_triggered();

    void on_actionClose_application_triggered();

private:
    Ui::C5MainWindow *ui;

    QLabel *fStatusLabel;

    QTabWidget *fTab;

    QToolBar *fReportToolbar;

    bool fLogin;

    void enableMenu(bool v);

    void addTreeL3Item(QTreeWidgetItem *item, int permission, const QString &text, const QString &icon);
};

#endif // C5MAINWINDOW_H
