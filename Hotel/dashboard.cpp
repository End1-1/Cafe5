#include "dashboard.h"
#include "ui_dashboard.h"
#include "login.h"
#include "message.h"
#include "chartwindow.h"
#include <QToolButton>
#include <QCloseEvent>
#include <QTimer>
#include <QTabBar>

Dashboard *__dashboard = nullptr;

template<typename T>
QWidget *createTabWidget() {
    T *t = new T();
    t->setWidgetContainer();
    __dashboard->fTabWidget->addTab(t, QIcon(t->icon()), t->title());
    return t;
}

Dashboard::Dashboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Dashboard)
{
    ui->setupUi(this);
    fTabWidget = ui->tw;
    QToolButton *btn = new QToolButton(ui->tw);
    btn->setMinimumSize(20, 20);
    btn->setIcon(QIcon(":/images/menu.png"));
    connect(btn, &QToolButton::clicked, this, &Dashboard::showMenu);
    ui->tw->setCornerWidget(btn, Qt::TopLeftCorner);
    btn->show();
    disableMenu();
}

Dashboard::~Dashboard()
{
    delete ui;
}

void Dashboard::hideMenu()
{
    QTimer *t = new QTimer();
    connect(t, &QTimer::timeout, this, &Dashboard::timeoutHideMenu);
    t->start(1);
}

void Dashboard::showMenu()
{
    if (ui->wMenu->isHidden()) {
        QTimer *t = new QTimer();
        connect(t, &QTimer::timeout, this, &Dashboard::timeoutShowMenu);
        t->start(1);
    } else {
        hideMenu();
    }
}

void Dashboard::disableMenu()
{
    ui->btnReservations->setVisible(false);
    ui->btnBills->setVisible(false);
    ui->btnLogin->setText(tr("Login"));
    ui->btnLogin->setProperty("login", true);
    ui->lwMenu->setMinimumHeight(0);
    ui->lwMenu->setMaximumHeight(0);
}

void Dashboard::prepareMenu()
{
    fActionsOfCashier.clear();
    fActionsOfReservations.clear();

    fActionsOfReservations.append(ui->actionRoom_chart);
    fActionsOfReservations.append(ui->actionNew_reservation);
    fActionsOfReservations.append(ui->actionNew_group_reservation);
    ui->btnReservations->setVisible(fActionsOfReservations.count() > 0);

    fActionsOfCashier.append(ui->actionIncomplete_bills);
    fActionsOfCashier.append(ui->actionBills);
    ui->btnBills->setVisible(fActionsOfCashier.count() > 0);

    ui->btnLogin->setText(tr("Logout"));
    ui->btnLogin->setProperty("login", false);
}

void Dashboard::buildMenu(QToolButton *btn, const QList<QAction *> &l)
{
    ui->lwMenu->clear();
    for (QAction *a: l)  {
        QToolButton *b = new QToolButton();
        b->setText(a->text());
        b->setIcon(a->icon());
        b->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        b->setStyleSheet("background-color: rgb(240, 240, 170); margin: 1px;");
        connect(b, SIGNAL(clicked()), a, SLOT(trigger()));
        QListWidgetItem *item = new QListWidgetItem(ui->lwMenu);
        item->setSizeHint(QSize(100, 30));
        ui->lwMenu->setItemWidget(item, b);
    }
    ui->lwMenu->setMinimumHeight(ui->lwMenu->count() * 30);
    ui->lwMenu->setMaximumHeight(ui->lwMenu->count() * 30);
    ui->verticalLayout->insertWidget(btn->property("index").toInt(), ui->lwMenu);
}

void Dashboard::timeoutHideMenu()
{
    if (ui->wMenu->width() > 10) {
        ui->wMenu->setMaximumSize(ui->wMenu->width() - 3, 16777215);
    } else {
        ui->wMenu->hide();
        static_cast<QTimer*>(sender())->deleteLater();
    }
}

void Dashboard::timeoutShowMenu()
{
    if (ui->wMenu->isHidden()) {
        ui->wMenu->show();
    }
    if (ui->wMenu->width() < 150) {
        ui->wMenu->setMaximumSize(ui->wMenu->width() + 3, 16777215);
    } else {
        static_cast<QTimer*>(sender())->deleteLater();
    }
}

void Dashboard::on_btnReservations_clicked()
{
    buildMenu(ui->btnReservations, fActionsOfReservations);
}

void Dashboard::on_actionNew_reservation_triggered()
{
    createTabWidget<ChartWindow>();
}

void Dashboard::on_tw_tabCloseRequested(int index)
{
    ui->tw->widget(index)->deleteLater();
    ui->tw->removeTab(index);
}

void Dashboard::on_btnBills_clicked()
{
    buildMenu(ui->btnBills, fActionsOfCashier);
}

void Dashboard::on_btnLogin_clicked()
{
    if (ui->btnLogin->property("login").toBool()) {
        if (Login::login()) {
            prepareMenu();
        }
    } else {
        if (Message::yesNo(tr("Confirm to logout")) == Message::rYes) {
            disableMenu();
        }
    }
}

void Dashboard::closeEvent(QCloseEvent *event)
{
    if (Message::yesNo(tr("Confirm to close application")) == Message::rNo) {
        event->ignore();
        return;
    }
    QWidget::closeEvent(event);
}
