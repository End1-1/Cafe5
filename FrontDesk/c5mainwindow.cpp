#include "c5mainwindow.h"
#include "ui_c5mainwindow.h"
#include "c5message.h"
#include "c5connection.h"
#include "c5login.h"
#include "c5permissions.h"
#include "c5widget.h"
#include "cr5commonsales.h"
#include "cr5usersgroups.h"
#include "cr5consumptionbysales.h"
#include "cr5documents.h"
#include "cr5dish.h"
#include "cr5settings.h"
#include "cr5goodsmovement.h"
#include "cr5creditcards.h"
#include "cr5dishpart1.h"
#include "cr5tstoreextra.h"
#include "cr5dishpart2.h"
#include "c5storeinventory.h"
#include "cr5discountsystem.h"
#include "cr5goodsgroup.h"
#include "cr5databases.h"
#include "cr5goodspartners.h"
#include "cr5goodswaste.h"
#include "c5welcomepage.h"
#include "cr5goodsunit.h"
#include "cr5menunames.h"
#include "cr5materialsinstore.h"
#include "cr5dishremovereason.h"
#include "cr5goods.h"
#include "cr5users.h"
#include "cr5goodsstorages.h"
#include "c5storedoc.h"
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
    ui->actionHome->setEnabled(false);

    QShortcut *f3 = new QShortcut(QKeySequence("Ctrl+F"), this);
    connect(f3, SIGNAL(activated()), this, SLOT(hotKey()));
    QShortcut *esc = new QShortcut(QKeySequence("ESC"), this);
    connect(esc, SIGNAL(activated()), this, SLOT(hotKey()));
    QShortcut *ctrlPlush = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus), this);
    connect(ctrlPlush, SIGNAL(activated()), this, SLOT(hotKey()));
    QShortcut *ctrlHome = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Home), this);
    connect(ctrlHome, SIGNAL(activated()), this, SLOT(on_actionGo_to_home_triggered()));
    QShortcut *ctrlLog = new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_L), this);
    connect(ctrlLog, &QShortcut::activated, [this]() {
        ui->wLog->setVisible(!ui->wLog->isVisible());
    });
    __mainWindow = this;

    QVariant menuPanelIsVisible = C5Config::getRegValue("menupanel");
    if (menuPanelIsVisible == QVariant::Invalid) {
        menuPanelIsVisible = true;
    }
    ui->twDb->setVisible(menuPanelIsVisible.toBool());
    ui->wLog->setVisible(false);
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

void C5MainWindow::removeTab(QWidget *w)
{
    for (int i = 0; i < fTab->count(); i++) {
        if (w == fTab->widget(i)) {
            tabCloseRequested(i);
            return;
        }
    }
}

void C5MainWindow::writeLog(const QString &message)
{
    ui->ptLog->appendPlainText(QString("%1: %2")
                               .arg(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR))
                                .arg(message));
}

void C5MainWindow::twCustomMenu(const QPoint &p)
{
    QTreeWidgetItem *item = ui->twDb->itemAt(p);
    QMenu menu(this);
    if (!item->parent()) {

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
    C5ReportWidget *w = dynamic_cast<C5ReportWidget*>(fTab->widget(index));
    C5Widget *w2 = dynamic_cast<C5Widget*>(fTab->widget(index));
    if (!w && !w2) {
        return;
    }
    if (w) {
        fReportToolbar = w->toolBar();
    } else if (w2) {
        fReportToolbar = w2->toolBar();
    }
    if (fReportToolbar) {
        addToolBar(fReportToolbar);
        fReportToolbar->show();
    }
}

void C5MainWindow::on_actionConnection_triggered()
{
    C5Connection::go<C5Connection>(C5Config::dbParams());
}

void C5MainWindow::on_actionLogin_triggered()
{
    if (!C5Connection::go<C5Login>(C5Config::dbParams())) {
        return;
    }
    fStatusLabel->setText(__username);

    C5Database db(C5Config::fDBHost, C5Config::fDBPath, C5Config::fDBUser, C5Config::fDBPassword);
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

        QVariant showwelcomePage = C5Config::getRegValue("showwelcomepage");
        if (showwelcomePage == QVariant::Invalid) {
            showwelcomePage = false;
        }
        if (showwelcomePage.toBool()) {
            showWelcomePage();
        }

        QTreeWidgetItem *it = 0;
        if (pr(db.getString(0), cp_t2_action)) {
            it = new QTreeWidgetItem();
            it->setText(0, tr("Actions"));
            it->setData(0, Qt::UserRole, cp_t2_action);
            it->setIcon(0, QIcon(":/edit.png"));
            item->addChild(it);
            addTreeL3Item(it, cp_t2_store_input, tr("New store input"), ":/goods.png");
            addTreeL3Item(it, cp_t2_store_output, tr("New store output"), ":/goods.png");
            addTreeL3Item(it, cp_t2_store_move, tr("New store movement"), ":/goods.png");
            addTreeL3Item(it, cp_t2_store_inventory, tr("New store inventory"), ":/goods.png");
        }

        if (pr(db.getString(0), cp_t3_reports)) {
            it = new QTreeWidgetItem();
            it->setText(0, tr("Reports"));
            it->setData(0, Qt::UserRole, cp_t2_action);
            it->setIcon(0, QIcon(":/reports.png"));
            item->addChild(it);
            addTreeL3Item(it, cp_t3_documents, tr("Documents"), ":/documents.png");
            addTreeL3Item(it, cp_t3_store, tr("Storage"), ":/goods.png");
            addTreeL3Item(it, cp_t3_store_movement, tr("Storages movements"), ":/goods.png");
            addTreeL3Item(it, cp_t3_tstore_extra, tr("T-account, extra"), ":/documents.png");
            addTreeL3Item(it, cp_t2_count_output_of_sale, tr("Consumption of goods based on sales"), ":/goods.png");
            addTreeL3Item(it, cp_t3_sales_common, tr("Sales, expert mode"), ":/graph.png");
        }

        if (pr(db.getString(0), cp_t4_menu)) {
            it = new QTreeWidgetItem();
            it->setText(0, tr("Menu"));
            it->setData(0, Qt::UserRole, cp_t4_part1);
            it->setIcon(0, QIcon(":/menu.png"));
            item->addChild(it);
            addTreeL3Item(it, cp_t4_part1, tr("Dish depts"), ":/menu.png");
            addTreeL3Item(it, cp_t4_part2, tr("Types of dishes"), ":/menu.png");
            addTreeL3Item(it, cp_t4_dishes, tr("Dishes list"), ":/menu.png");
            addTreeL3Item(it, cp_t4_menu_names, tr("Menu names"), ":/menu.png");
            addTreeL3Item(it, cp_t4_dish_remove_reason, tr("Dish remove reasons"), ":/menu.png");
        }

        if (pr(db.getString(0), cp_t6_goods_menu)) {
            it = new QTreeWidgetItem();
            it->setText(0, tr("Goods"));
            it->setData(0, Qt::UserRole, cp_t6_goods_menu);
            it->setIcon(0, QIcon(":/goods.png"));
            item->addChild(it);
            addTreeL3Item(it, cp_t6_storage, tr("Storages"), ":/goods.png");
            addTreeL3Item(it, cp_t6_groups, tr("Groups of goods"), ":/goods.png");
            addTreeL3Item(it, cp_t6_goods, tr("Goods"), ":/goods.png");
            addTreeL3Item(it, cp_t6_waste, tr("Autowaste"), ":/goods.png");
            addTreeL3Item(it, cp_t6_units, tr("Units"), ":/goods.png");
            addTreeL3Item(it, cp_t6_partners, tr("Partners"), ":/goods.png");
        }

        if (pr(db.getString(0), cp_t7_other)) {
            it = new QTreeWidgetItem();
            it->setText(0, tr("Other"));
            it->setData(0, Qt::UserRole, cp_t6_goods_menu);
            it->setIcon(0, QIcon(":/other.png"));
            item->addChild(it);
            addTreeL3Item(it, cp_t7_credit_card, tr("Credit cards"), ":/credit-card.png");
            addTreeL3Item(it, cp_t7_discount_system, tr("Discount system"), ":/discount.png");
        }

        if (pr(db.getString(0), cp_t1_preference)) {
            it = new QTreeWidgetItem();
            it->setText(0, tr("Preferences"));
            it->setData(0, Qt::UserRole, cp_t1_preference);
            it->setIcon(0, QIcon(":/configure.png"));
            item->addChild(it);
            addTreeL3Item(it, cp_t1_usergroups, tr("Users groups"), ":/users_groups.png");
            addTreeL3Item(it, cp_t1_users, tr("Users"), ":/users_groups.png");
            addTreeL3Item(it, cp_t1_databases, tr("Databases"), ":/database.png");
            addTreeL3Item(it, cp_t1_settigns, tr("Settings"), ":/configure.png");
        }
    }
    if (ui->twDb->topLevelItemCount() == 1) {
        ui->twDb->expandAll();
    }
    enableMenu(true);
    ui->actionHome->setEnabled(true);
    ui->actionLogin->setVisible(false);
}

void C5MainWindow::hotKey()
{
    QShortcut *s = static_cast<QShortcut*>(sender());
    C5Widget *w = static_cast<C5Widget*>(fTab->currentWidget());
    if (!w) {
        return;
    }
    w->hotKey(s->key().toString());
}

void C5MainWindow::enableMenu(bool v)
{
    if (!v) {
        ui->twDb->clear();
    }
    fLogin = v;
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

void C5MainWindow::showWelcomePage()
{
    QTreeWidgetItem *item = ui->twDb->topLevelItem(0);
    QStringList dbparams;
    dbparams << item->data(0, Qt::UserRole + 1).toString()
             << item->data(0, Qt::UserRole + 2).toString()
             << item->data(0, Qt::UserRole + 3).toString()
             << item->data(0, Qt::UserRole + 4).toString()
             << item->data(0, Qt::UserRole).toString();
    C5WelcomePage *t = new C5WelcomePage(dbparams);
    fTab->insertTab(0, t, t->icon(), QString("[%1] %2").arg(dbparams.at(4)).arg(t->label()));
    fTab->setCurrentIndex(0);
    fTab->tabBar()->tabButton(0, QTabBar::RightSide)->resize(0, 0);
    t->postProcess();
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
    case cp_t1_databases:
        createTab<CR5Databases>(dbParams);
        break;
    case cp_t1_settigns:
        createTab<CR5Settings>(dbParams);
        break;
    case cp_t2_store_input: {
        C5StoreDoc *sd = createTab<C5StoreDoc>(dbParams);
        sd->setMode(C5StoreDoc::sdInput);
        break;
    }
    case cp_t2_store_output: {
        C5StoreDoc *sd = createTab<C5StoreDoc>(dbParams);
        sd->setMode(C5StoreDoc::sdOutput);
        break;
    }
    case cp_t2_store_move: {
        C5StoreDoc *sd = createTab<C5StoreDoc>(dbParams);
        sd->setMode(C5StoreDoc::sdMovement);
        break;
    }
    case cp_t2_store_inventory:
        createTab<C5StoreInventory>(dbParams);
        break;
    case cp_t2_count_output_of_sale:
        createTab<CR5ConsumptionBySales>(dbParams);
        break;
    case cp_t3_sales_common:
        createTab<CR5CommonSales>(dbParams);
        break;
    case cp_t3_documents:
        createTab<CR5Documents>(dbParams);
        break;
    case cp_t3_store:
        createTab<CR5MaterialsInStore>(dbParams);
        break;
    case cp_t3_store_movement:
        createTab<CR5GoodsMovement>(dbParams);
        break;
    case cp_t3_tstore_extra:
        createTab<CR5TStoreExtra>(dbParams);
        break;
    case cp_t4_part1:
        createTab<CR5DishPart1>(dbParams);
        break;
    case cp_t4_part2:
        createTab<CR5DishPart2>(dbParams);
        break;
    case cp_t4_dishes:
        createTab<CR5Dish>(dbParams);
        break;
    case cp_t4_menu_names:
        createTab<CR5MenuNames>(dbParams);
        break;
    case cp_t4_dish_remove_reason:
        createTab<CR5DishRemoveReason>(dbParams);
        break;
    case cp_t6_units:
        createTab<CR5GoodsUnit>(dbParams);
        break;
    case cp_t6_groups:
        createTab<CR5GoodsGroup>(dbParams);
        break;
    case cp_t6_goods:
        createTab<CR5Goods>(dbParams);
        break;
    case cp_t6_waste:
        createTab<CR5GoodsWaste>(dbParams);
        break;
    case cp_t6_storage:
        createTab<CR5GoodsStorages>(dbParams);
        break;
    case cp_t6_partners:
        createTab<CR5GoodsPartners>(dbParams);
        break;
    case cp_t7_credit_card:
        createTab<CR5CreditCards>(dbParams);
        break;
    case cp_t7_discount_system:
        createTab<CR5DiscountSystem>(dbParams);
        break;
    default:
        break;
    }
}

void C5MainWindow::on_actionClose_application_triggered()
{
    if (C5Message::question(tr("Are you sure to close application?")) == QDialog::Accepted) {
        qApp->quit();
    }
}

void C5MainWindow::on_btnHideMenu_clicked()
{
    ui->twDb->setVisible(!ui->twDb->isVisible());
    ui->btnHideMenu->setText(ui->twDb->isVisible() ? "<" : ">");
    C5Config::setRegValue("menupanel", ui->twDb->isVisible());
}
void C5MainWindow::on_actionHome_triggered()
{
    bool isVisible = fTab->count() > 0;
    C5WelcomePage *wp = 0;
    if (isVisible) {
        wp = dynamic_cast<C5WelcomePage*>(fTab->widget(0));
    }
    isVisible = wp != 0;
    if (isVisible) {
        removeTab(wp);
    } else {
        if (ui->twDb->topLevelItemCount() > 0) {
            showWelcomePage();
        }
    }
    C5Config::setRegValue("showwelcomepage", !isVisible);
}

void C5MainWindow::on_actionGo_to_home_triggered()
{
    if (fTab->count() == 0) {
        showWelcomePage();
        return;
    }
    C5WelcomePage *wp = dynamic_cast<C5WelcomePage*>(fTab->widget(0));
    if (wp) {
        fTab->setCurrentIndex(0);
    } else {
        showWelcomePage();
    }
}
