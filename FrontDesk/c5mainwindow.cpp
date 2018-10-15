#include "c5mainwindow.h"
#include "ui_c5mainwindow.h"
#include "c5message.h"
#include "c5connection.h"
#include "c5login.h"
#include "c5permissions.h"
#include "cr5commonsales.h"
#include "cr5usersgroups.h"
#include "cr5users.h"
#include <QCloseEvent>
#include <QShortcut>
#include <QMenu>

C5MainWindow::C5MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::C5MainWindow)
{
    ui->setupUi(this);
    ui->splitter->setStretchFactor(0, 0);
    ui->splitter->setStretchFactor(1, 1);
    __c5config.fParentWidget = this;
    fStatusLabel = new QLabel(tr("Disconnected"));
    ui->statusBar->addWidget(fStatusLabel);
    ui->twDb->setContextMenuPolicy(Qt::CustomContextMenu);
    fTab = ui->tabWidget;
    enableMenu(false);
    connect(ui->twDb, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(twCustomMenu(QPoint)));
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequested(int)));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(currentTabChange(int)));
    fReportToolbar = 0;
    QShortcut *f3 = new QShortcut(QKeySequence("Ctrl+F"), this);
    connect(f3, &QShortcut::activated, [this]() {
        C5ReportWidget *rw = static_cast<C5ReportWidget*>(fTab->currentWidget());
        rw->hotkey("F3");
    });
    QShortcut *esc = new QShortcut(QKeySequence("ESC"), this);
    connect(esc, &QShortcut::activated, [this]() {
        C5ReportWidget *rw = static_cast<C5ReportWidget*>(fTab->currentWidget());
        rw->hotkey("esc");
    });
}

C5MainWindow::~C5MainWindow()
{
    delete ui;
}

void C5MainWindow::closeEvent(QCloseEvent *event)
{
    if (C5Message::question(tr("Are you sure to close application?")) != QDialog::Accepted) {
        event->ignore();
        return;
    }
    QMainWindow::closeEvent(event);
}

void C5MainWindow::twCustomMenu(const QPoint &p)
{
    QTreeWidgetItem *item = ui->twDb->itemAt(p);
    QMenu menu(this);
    if (!item->parent()) {
        menu.addAction(ui->actionConfigure_connection);
    }
    menu.exec(ui->twDb->mapToGlobal(p));
}

void C5MainWindow::tabCloseRequested(int index)
{
    C5ReportWidget *w = static_cast<C5ReportWidget*>(fTab->widget(index));
    fTab->removeTab(index);
    delete w;
}

void C5MainWindow::currentTabChange(int index)
{
    if (fReportToolbar) {
        removeToolBar(fReportToolbar);
    }
    fReportToolbar = 0;
    C5ReportWidget *w = static_cast<C5ReportWidget*>(fTab->widget(index));
    if (!w) {
        return;
    }
    fReportToolbar = w->toolBar();
    addToolBar(fReportToolbar);
    fReportToolbar->show();
}

void C5MainWindow::on_actionConnection_triggered()
{
    C5Connection::go<C5Connection>();
}

void C5MainWindow::on_actionLogin_triggered()
{
    if (!C5Connection::go<C5Login>()) {
        return;
    }
    fStatusLabel->setText(__username);

    C5Database db;
    db[":f_user"] = __userid;
    db.exec("select f_db, f_description, f_host, f_db, f_user, f_password, f_main from s_db \
            where f_name in (select f_db from s_db_access where f_user=:f_user and f_permit=1)");
    while (db.nextRow()) {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, db.getString(0));
        item->setToolTip(0, db.getString(1));
        item->setData(0, Qt::UserRole, db.getString(1));
        item->setData(0, Qt::UserRole + 1, db.getString(2));
        item->setData(0, Qt::UserRole + 2, db.getString(3));
        item->setData(0, Qt::UserRole + 3, db.getString(4));
        item->setData(0, Qt::UserRole + 4, db.getString(5));
        item->setData(0, Qt::UserRole + 5, db.getInt(6));
        QString icon = db.getInt(6) == 0 ? ":/database.png" : ":/database_main.png";
        item->setIcon(0, QIcon(icon));
        ui->twDb->addTopLevelItem(item);

        QTreeWidgetItem *it = 0;
        if (pr(db.getString(0), cp_t2_action)) {
            it = new QTreeWidgetItem();
            it->setText(0, tr("Actions"));
            it->setData(0, Qt::UserRole, cp_t2_action);
            it->setIcon(0, QIcon(":/edit.png"));
            item->addChild(it);
        }

        if (pr(db.getString(0), cp_t3_reports)) {
            it = new QTreeWidgetItem();
            it->setText(0, tr("Reports"));
            it->setData(0, Qt::UserRole, cp_t2_action);
            it->setIcon(0, QIcon(":/reports.png"));
            item->addChild(it);
            addTreeL3Item(it, cp_t3_sales_common, tr("Sales, common"), ":/graph.png");
        }

        if (pr(db.getString(0), cp_t1_preference)) {
            it = new QTreeWidgetItem();
            it->setText(0, tr("Preferences"));
            it->setData(0, Qt::UserRole, cp_t1_preference);
            it->setIcon(0, QIcon(":/configure.png"));
            item->addChild(it);
            addTreeL3Item(it, cp_t1_usergroups, tr("Users groups"), ":/users_groups.png");
            addTreeL3Item(it, cp_t1_users, tr("Users"), ":/users_groups.png");
        }
    }

    enableMenu(true);
}

void C5MainWindow::enableMenu(bool v)
{
    if (!v) {
        ui->twDb->clear();
    }
    fLogin = v;
    ui->actionAppend_database->setEnabled(v && __usergroup == 1);
}

void C5MainWindow::addTreeL3Item(QTreeWidgetItem *item, int permission, const QString &text, const QString &icon)
{
    QTreeWidgetItem *root = item;
    while (root->parent() != 0) {
        root = root->parent();
    }
    QString db = root->text(0);
    if (!pr(db, permission)) {
        return;
    }
    QTreeWidgetItem *child = new QTreeWidgetItem();
    child->setText(0, text);
    child->setIcon(0, QIcon(icon));
    child->setData(0, Qt::UserRole, permission);
    item->addChild(child);
}

void C5MainWindow::on_twDb_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    if (!item->parent()) {
        return;
    }
    if (!item->parent()->parent()) {
        return;
    }
    QTreeWidgetItem *root = item->parent()->parent();
    QStringList dbParams;
    dbParams << root->data(0, Qt::UserRole + 1).toString()
             << root->data(0, Qt::UserRole + 2).toString()
             << root->data(0, Qt::UserRole + 3).toString()
             << root->data(0, Qt::UserRole + 4).toString()
             << root->data(0, Qt::UserRole).toString();
    switch (item->data(0, Qt::UserRole).toInt()) {
    case cp_t1_usergroups:
        createTab<CR5UsersGroups>(dbParams);
        break;
    case cp_t1_users:
        createTab<CR5Users>(dbParams);
        break;
    case cp_t3_sales_common:
        createTab<CR5CommonSales>(dbParams);
        break;
    default:
        break;
    }
}

void C5MainWindow::on_actionConfigure_connection_triggered()
{

}

void C5MainWindow::on_actionClose_application_triggered()
{
    if (C5Message::question(tr("Are you sure to close application?")) == QDialog::Accepted) {
        qApp->quit();
    }
}
