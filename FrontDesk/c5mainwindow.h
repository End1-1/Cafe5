#ifndef C5MAINWINDOW_H
#define C5MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTreeWidgetItem>
#include <QTimer>

namespace Ui {
class C5MainWindow;
}

class C5ToolBarWidget;

class C5MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit C5MainWindow(QWidget *parent = nullptr);

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

    void removeTab(QWidget *w);

    void writeLog(const QString &message);

public slots:
    void on_actionLogin_triggered();

private slots:
    void updateTimeout();

    void updateChecked(bool needUpdate, int source, const QString &path);

    void hotKey();

    void twCustomMenu(const QPoint &p);

    void tabCustomMenu(const QPoint &p);

    void tabCloseRequested(int index);

    void actionChangeDatabase();

    void currentTabChange(int index);

    void on_actionConnection_triggered();

    void on_twDb_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_actionClose_application_triggered();

    void on_btnHideMenu_clicked();

    void on_actionHome_triggered();

    void on_actionGo_to_home_triggered();

    void on_twDb_itemExpanded(QTreeWidgetItem *item);

    void on_actionLogout_triggered();

    void on_actionChange_password_triggered();

private:
    Ui::C5MainWindow *ui;

    QLabel *fStatusLabel;

    QTabWidget *fTab;

    QToolBar *fReportToolbar;

    QToolBar *fRightToolbar;

    C5ToolBarWidget *fToolbarWidget;

    QTimer fUpdateTimer;

    bool fLogin;

    void enableMenu(bool v);

    void addTreeL3Item(QTreeWidgetItem *item, int permission, const QString &text, const QString &icon);

    void showWelcomePage();

    QStringList getDbParams(QTreeWidgetItem *item);
};

extern C5MainWindow *__mainWindow;

#endif // C5MAINWINDOW_H
