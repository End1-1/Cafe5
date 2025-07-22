#ifndef C5MAINWINDOW_H
#define C5MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTreeWidgetItem>
#include <QTimer>
#include <QListWidgetItem>
#include <QUdpSocket>
#include <QJsonObject>

namespace Ui
{
class C5MainWindow;
}

class C5ToolBarWidget;
class QListWidget;
class C5Widget;
class NTableWidget;
class NInterface;
class QPushButton;

class C5MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit C5MainWindow(QWidget *parent = nullptr);

    ~C5MainWindow();

    virtual void closeEvent(QCloseEvent *event);

    template<typename T>
    T* createTab()
    {
        T *t = new T();
        fTab->addTab(t, t->icon(), QString("%1").arg(t->label()));
        fTab->setCurrentIndex(fTab->count() - 1);
        t->postProcess();
        return t;
    }

    NTableWidget* createNTab(const QString &route, const QString &image, const QJsonObject &initParams = QJsonObject{});

    void nTabDesign(const QIcon &icon, const QString &label, NTableWidget *widget);

    void removeTab(QWidget *w);

    void addBroadcastListener(C5Widget *w);

    void removeBroadcastListener(C5Widget *w);

public slots:
    void on_actionLogin_triggered();

private slots:
    void menuListReponse(const QJsonObject &jdoc);

    void updateTimeout();

    void hotKey();

    void tabCloseRequested(int index);

    void currentTabChange(int index);

    void on_listWidgetItemClicked(const QModelIndex &index);

    void on_actionClose_application_triggered();

    void on_actionLogout_triggered();

    void on_actionChange_password_triggered();

    void on_splitter_splitterMoved(int pos, int index);

    void on_btnHideMenu_clicked();

    void on_btnMenuClick();

    void on_btnFavoriteClicked();

    void on_btnShowMenu_clicked();

private:
    NInterface* http;

    Ui::C5MainWindow* ui;

    QList<QListWidget*> fMenuLists;

    QLabel* fStatusLabel;

    QPushButton* fConnectionLabel;

    QStringList fPrevTabUuid;

    QTabWidget* fTab;

    QMap<QString, C5Widget*> fBroadcastListeners;

    QToolBar* fReportToolbar;

    QToolBar* fRightToolbar;

    C5ToolBarWidget* fToolbarWidget;

    QTimer fTimer;

    bool fLogin;

    void readFavoriteMenu();

    bool addMainLevel(const QString &db, int permission, const QString &title, const QString &icon, QListWidget*& l);

    void setDB();

    void enableMenu(bool v);

    QListWidgetItem* addTreeL3Item(QListWidget *l, int permission, const QString &text, const QString &icon);

    void animateMenu(QListWidget *l, bool hide);

    QString itemIconName(int permission);

    void removeFromFavorite(int permission);
};

extern C5MainWindow* __mainWindow;

#endif // C5MAINWINDOW_H
