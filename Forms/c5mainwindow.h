#ifndef C5MAINWINDOW_H
#define C5MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTreeWidgetItem>
#include <QTimer>
#include <QUdpSocket>

namespace Ui {
class C5MainWindow;
}

class C5ToolBarWidget;
class QListWidget;
class C5Widget;

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
        fTab->addTab(t, t->icon(), QString("[%1] %2").arg(dbParams.at(5)).arg(t->label()));
        fTab->setCurrentIndex(fTab->count() - 1);
        t->postProcess();
        return t;
    }

    void removeTab(QWidget *w);

    void writeLog(const QString &message);

    void addBroadcastListener(C5Widget *w);

    void removeBroadcastListener(C5Widget *w);

public slots:
    void on_actionLogin_triggered();

private slots:
    void updateTimeout();

    void updateChecked(bool needUpdate, int source, const QString &path);

    void hotKey();

    void tabCloseRequested(int index);

    void currentTabChange(int index);

    void on_actionConnection_triggered();

    void on_listWidgetItemClicked(const QModelIndex &index);

    void on_actionClose_application_triggered();

    void on_actionLogout_triggered();

    void on_actionChange_password_triggered();

    void on_splitter_splitterMoved(int pos, int index);

    void on_btnHideMenu_clicked();

    void on_btnChangeDB_clicked();

    void on_btnMenuClick();

    void on_btnFavoriteClicked();

private:
    Ui::C5MainWindow *ui;

    QList<QListWidget*> fMenuLists;

    QMap<QString, QStringList> fDatabases;

    QLabel *fStatusLabel;

    QStringList fPrevTabUuid;

    QTabWidget *fTab;

    QMap<QString, C5Widget *> fBroadcastListeners;

    QToolBar *fReportToolbar;

    QToolBar *fRightToolbar;

    C5ToolBarWidget *fToolbarWidget;

    QTimer fUpdateTimer;

    bool fLogin;

    void readFavoriteMenu();

    bool addMainLevel(const QString &db, int permission, const QString &title, const QString &icon, QListWidget *&l);

    void setDB(const QString &dbname);

    void enableMenu(bool v);

    void addTreeL3Item(QListWidget *l, int permission, const QString &text, const QString &icon);

    void animateMenu(QListWidget *l, bool hide);

    QString itemIconName(int permission);

    void removeFromFavorite(int permission);

    void autoLogin();
};

extern C5MainWindow *__mainWindow;

#endif // C5MAINWINDOW_H
