#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>

namespace Ui {
class Dashboard;
}

class QToolButton;
class QTabWidget;

class Dashboard : public QWidget
{
    Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = nullptr);
    ~Dashboard();
    QTabWidget *fTabWidget;

private:
    Ui::Dashboard *ui;
    QList<QAction*> fActionsOfReservations;
    QList<QAction*> fActionsOfCashier;
    void hideMenu();
    void showMenu();
    void disableMenu();
    void prepareMenu();
    void buildMenu(QToolButton *btn, const QList<QAction*> &l);

public slots:
    void on_btnLogin_clicked();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void timeoutHideMenu();
    void timeoutShowMenu();
    void on_btnReservations_clicked();
    void on_actionNew_reservation_triggered();
    void on_tw_tabCloseRequested(int index);
    void on_btnBills_clicked();

};

extern Dashboard *__dashboard;

#endif // DASHBOARD_H
