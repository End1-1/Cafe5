#include "c5mainwindow.h"
#include "ui_c5mainwindow.h"
#include "c5message.h"
#include "c5connection.h"
#include "c5login.h"
#include "c5cashdoc.h"
#include "cr5cashdetailed.h"
#include "c5permissions.h"
#include "c5widget.h"
#include "cr5commonsales.h"
#include "checkforupdatethread.h"
#include "cr5usersgroups.h"
#include "cr5consumptionbysales.h"
#include "cr5storereason.h"
#include "cr5documents.h"
#include "cr5goodsclasses.h"
#include "c5translatorform.h"
#include "cr5saleremoveddishes.h"
#include "c5costumerdebtpayment.h"
#include "cr5cashnames.h"
#include "cr5dish.h"
#include "cr5settings.h"
#include "cr5goodsmovement.h"
#include "c5salarydoc.h"
#include "cr5creditcards.h"
#include "cr5debtstopartner.h"
#include "cr5dishpart1.h"
#include "c5toolbarwidget.h"
#include "c5dishselfcostgenprice.h"
#include "cr5salefromstoretotal.h"
#include "cr5dishcomment.h"
#include "cr5salesbydishes.h"
#include "cr5tstoreextra.h"
#include "cr5costumerdebts.h"
#include "cr5storedocuments.h"
#include "cr5dishpart2.h"
#include "cr5dishpriceselfcost.h"
#include "c5storeinventory.h"
#include "cr5discountsystem.h"
#include "cr5dishpackage.h"
#include "cr5goodsgroup.h"
#include "cr5menureview.h"
#include "cr5databases.h"
#include "cr5consuptionreason.h"
#include "cr5storereason.h"
#include "cr5salarybyworkers.h"
#include "cr5goodspartners.h"
#include "cr5discountstatisics.h"
#include "cr5goodswaste.h"
#include "c5welcomepage.h"
#include "cr5salefromstore.h"
#include "cr5goodsunit.h"
#include "c5datasynchronize.h"
#include "cr5customers.h"
#include "cr5menunames.h"
#include "cr5tables.h"
#include "cr5hall.h"
#include "cr5materialsinstore.h"
#include "c5changepassword.h"
#include "cr5dishremovereason.h"
#include "cr5goods.h"
#include "cr5users.h"
#include "cr5goodsstorages.h"
#include "c5storedoc.h"
#include <QCloseEvent>
#include <QShortcut>
#include <QProcess>
#include <QMenu>

C5MainWindow *__mainWindow;

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
    connect(ui->tabWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(tabCustomMenu(QPoint)));
    fReportToolbar = nullptr;
    fRightToolbar = new QToolBar();
    addToolBar(fRightToolbar);
    fToolbarWidget = new C5ToolBarWidget(this);
    fToolbarWidget->setUpdateButtonVisible(false);
    fRightToolbar->addWidget(fToolbarWidget);
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
    C5Dialog::setMainWindow(this);
    __mainWindow = this;

    QVariant menuPanelIsVisible = C5Config::getRegValue("menupanel");
    if (menuPanelIsVisible == QVariant::Invalid) {
        menuPanelIsVisible = true;
    }
    ui->twDb->setVisible(menuPanelIsVisible.toBool());
    ui->wLog->setVisible(false);
    ui->actionLogout->setVisible(false);
    ui->actionChange_password->setVisible(false);
    connect(&fUpdateTimer, SIGNAL(timeout()), this, SLOT(updateTimeout()));
    fUpdateTimer.start(10000);
    ui->twDb->resize(C5Config::getRegValue("twdbsize", 300).toInt(), 0);
}

C5MainWindow::~C5MainWindow()
{
    fUpdateTimer.stop();
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

void C5MainWindow::tabCustomMenu(const QPoint &p)
{
    C5Widget *w = static_cast<C5Widget*>(ui->tabWidget->currentWidget());
    if (w) {
        if (!w->allowChangeDatabase()) {
            return;
        }
    }
    QMenu menu(this);
    for (int i = 0; i < ui->twDb->topLevelItemCount(); i++) {
        QAction *a = menu.addAction(ui->twDb->topLevelItem(i)->text(0), this, SLOT(actionChangeDatabase()));
        a->setData(i);
    }
    menu.exec(ui->tabWidget->mapToGlobal(p));
}

void C5MainWindow::tabCloseRequested(int index)
{
    C5ReportWidget *w = static_cast<C5ReportWidget*>(fTab->widget(index));
    fTab->removeTab(index);
    delete w;
}

void C5MainWindow::actionChangeDatabase()
{
    QAction * a = static_cast<QAction*>(sender());
    int index = a->data().toInt();
    QTreeWidgetItem *item = ui->twDb->topLevelItem(index);
    if (!item) {
        return;
    }
    QStringList dbParams;
    dbParams << item->data(0, Qt::UserRole + 1).toString()
             << item->data(0, Qt::UserRole + 2).toString()
             << item->data(0, Qt::UserRole + 3).toString()
             << item->data(0, Qt::UserRole + 4).toString()
             << item->data(0, Qt::UserRole).toString();
    C5Widget *w = static_cast<C5Widget*>(ui->tabWidget->currentWidget());
    if (!w) {
        return;
    }
    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), QString("[%1] %2").arg(dbParams.at(4)).arg(w->label()));
    w->changeDatabase(dbParams);
}

void C5MainWindow::currentTabChange(int index)
{
    if (fReportToolbar) {
        removeToolBar(fReportToolbar);
    }
    fReportToolbar = nullptr;
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
        insertToolBar(fRightToolbar, fReportToolbar);
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
    showMaximized();
    fStatusLabel->setText(__username);

    C5Database db(C5Config::fDBHost, C5Config::fDBPath, C5Config::fDBUser, C5Config::fDBPassword);
    db[":f_user"] = __userid;
    db.exec("select f_name, f_description, f_host, f_db, f_user, f_password, f_main from s_db "
            "where f_id in (select f_db from s_db_access where f_user=:f_user and f_permit=1)");
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

        C5Database dbpr(db.getString(2),  db.getString(3),  db.getString(4),  db.getString(5));
        C5Permissions::init(dbpr);

        QVariant showwelcomePage = C5Config::getRegValue("showwelcomepage");
        if (showwelcomePage == QVariant::Invalid) {
            showwelcomePage = false;
        }
        if (showwelcomePage.toBool()) {
            bool isVisible = fTab->count() > 0;
            C5WelcomePage *wp = nullptr;
            if (isVisible) {
                wp = dynamic_cast<C5WelcomePage*>(fTab->widget(0));
            }
            if (wp == nullptr) {
                showWelcomePage();
            }
        }

        QTreeWidgetItem *it = nullptr;
        if (pr(db.getString(3), cp_t2_action)) {
            it = new QTreeWidgetItem();
            it->setText(0, tr("Actions"));
            it->setData(0, Qt::UserRole, cp_t2_action);
            it->setIcon(0, QIcon(":/edit.png"));
            item->addChild(it);
            addTreeL3Item(it, cp_t2_store_input, tr("New store input"), ":/goods.png");
            addTreeL3Item(it, cp_t2_store_output, tr("New store output"), ":/goods.png");
            addTreeL3Item(it, cp_t2_store_move, tr("New store movement"), ":/goods.png");
            addTreeL3Item(it, cp_t2_store_complectation, tr("New store complecation"), ":/goods.png");
            addTreeL3Item(it, cp_t2_store_inventory, tr("New store inventory"), ":/goods.png");
            if (__c5config.frontDeskMode() == FRONTDESK_WAITER) {
                addTreeL3Item(it, cp_t2_calculate_self_cost, tr("Calculate dishes self cost"), ":/menu.png");
            }
        }

        if (pr(db.getString(3), cp_t3_reports)) {
            it = new QTreeWidgetItem();
            it->setText(0, tr("Reports"));
            it->setData(0, Qt::UserRole, cp_t2_action);
            it->setIcon(0, QIcon(":/reports.png"));
            item->addChild(it);
            addTreeL3Item(it, cp_t3_documents, tr("Documents"), ":/documents.png");
            addTreeL3Item(it, cp_t3_documents_store, tr("Documents in the store"), ":/documents.png");
            addTreeL3Item(it, cp_t3_store, tr("Storage"), ":/goods.png");
            addTreeL3Item(it, cp_t3_store_movement, tr("Storages movements"), ":/goods.png");
            addTreeL3Item(it, cp_t3_sale_from_store_total, tr("Detailed movement in the storage"), ":/graph.png");
            addTreeL3Item(it, cp_t3_tstore_extra, tr("T-account, extra"), ":/documents.png");
            addTreeL3Item(it, cp_t2_count_output_of_sale, tr("Consumption of goods based on sales"), ":/goods.png");
            addTreeL3Item(it, cp_t3_consuption_reason, tr("Reason for consuption"), ":/goods.png");
            addTreeL3Item(it, cp_t3_sales_common, tr("Sales, expert mode"), ":/graph.png");
            addTreeL3Item(it, cp_t3_sale_dishes, tr("Sales, dishes"), ":/graph.png");
            addTreeL3Item(it, cp_t3_sale_removed_dishes, tr("Sales, removed dishes"), ":/delete.png");
            addTreeL3Item(it, cp_t3_store_sale, tr("Sales from store"), ":/graph.png");
            addTreeL3Item(it, cp_t3_debts_to_partners, tr("Debts to partners"), ":/contract.png");
            addTreeL3Item(it, cp_t3_discount_statistics, tr("Discount statistics"), ":/discount.png");
        }

        if (pr(db.getString(3), cp_t8_cash)) {
            it = new QTreeWidgetItem();
            it->setText(0, tr("Cash"));
            it->setData(0, Qt::UserRole, cp_t2_action);
            it->setIcon(0, QIcon(":/reports.png"));
            item->addChild(it);
            addTreeL3Item(it, cp_t8_cash_doc, tr("New cash document"), ":/cash.png");
            addTreeL3Item(it, cp_t8_costumer_debts_pay, tr("New payment for costumer debt"), ":/cash.png");
            addTreeL3Item(it, cp_t8_cash_detailed_report, tr("Cash detailed report"), ":/cash.png");
            addTreeL3Item(it, cp_t8_costumer_debts, tr("Costumers debts report"), ":/cash.png");
            addTreeL3Item(it, cp_t8_cash_names, tr("Cash names"), ":/cash.png");
        }

        if (pr(db.getString(3), cp_t9_salary)) {
            it = new QTreeWidgetItem();
            it->setText(0, tr("Salary"));
            it->setData(0, Qt::UserRole, cp_t9_salary);
            it->setIcon(0, QIcon(":/employee.png"));
            item->addChild(it);
            addTreeL3Item(it, cp_t9_salary_doc, tr("New salary document"), ":/employee.png");
            addTreeL3Item(it, cp_t9_report, tr("Salary by workers"), ":/employee.png");
        }

        if (__c5config.frontDeskMode() == FRONTDESK_WAITER) {
            if (pr(db.getString(3), cp_t4_menu)) {
                it = new QTreeWidgetItem();
                it->setText(0, tr("Menu"));
                it->setData(0, Qt::UserRole, cp_t4_part1);
                it->setIcon(0, QIcon(":/menu.png"));
                item->addChild(it);
                addTreeL3Item(it, cp_t4_part1, tr("Dish depts"), ":/menu.png");
                addTreeL3Item(it, cp_t4_part2, tr("Types of dishes"), ":/menu.png");
                addTreeL3Item(it, cp_t4_dishes, tr("Dishes list"), ":/menu.png");
                addTreeL3Item(it, cp_t4_dishes_packages, tr("Dishes packages"), ":/menu.png");
                addTreeL3Item(it, cp_t4_menu_names, tr("Menu names"), ":/menu.png");
                addTreeL3Item(it, cp_t4_dish_remove_reason, tr("Dish remove reasons"), ":/menu.png");
                addTreeL3Item(it, cp_t4_dish_comments, tr("Dish comments"), ":/menu.png");
                addTreeL3Item(it, cp_t4_dish_price_self_cost, tr("Dish self cost report"), ":/menu.png");
                addTreeL3Item(it, cp_t4_menu_review, tr("Review menu"), ":/menu.png");
            }
        }

        if (pr(db.getString(3), cp_t6_goods_menu)) {
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
            addTreeL3Item(it, cp_t6_classes, tr("Classes"), ":/goods.png");
        }

        if (pr(db.getString(3), cp_t7_other)) {
            it = new QTreeWidgetItem();
            it->setText(0, tr("Other"));
            it->setData(0, Qt::UserRole, cp_t6_goods_menu);
            it->setIcon(0, QIcon(":/other.png"));
            item->addChild(it);
            addTreeL3Item(it, cp_t7_halls, tr("Halls"), ":/hall.png");
            addTreeL3Item(it, cp_t7_tables, tr("Tables"), ":/table.png");
            addTreeL3Item(it, cp_t7_credit_card, tr("Credit cards"), ":/credit-card.png");
            addTreeL3Item(it, cp_t7_customers, tr("Customers"), ":/customers.png");
            addTreeL3Item(it, cp_t7_discount_system, tr("Discount system"), ":/discount.png");
            addTreeL3Item(it, cp_t7_store_reason, tr("Store reason"), ":/documents.png");
        }

        if (pr(db.getString(3), cp_t1_preference)) {
            it = new QTreeWidgetItem();
            it->setText(0, tr("Preferences"));
            it->setData(0, Qt::UserRole, cp_t1_preference);
            it->setIcon(0, QIcon(":/configure.png"));
            item->addChild(it);
            addTreeL3Item(it, cp_t1_usergroups, tr("Users groups"), ":/users_groups.png");
            addTreeL3Item(it, cp_t1_users, tr("Users"), ":/users_groups.png");
            addTreeL3Item(it, cp_t1_databases, tr("Databases"), ":/database.png");
            addTreeL3Item(it, cp_t1_settigns, tr("Settings"), ":/configure.png");
            addTreeL3Item(it, cp_t7_translator, tr("Translator"), ":/translate.png");
        }
    }
    if (ui->twDb->topLevelItemCount() == 1) {
        ui->twDb->expandAll();
    }
    enableMenu(true);
    ui->actionHome->setEnabled(true);
    ui->actionLogin->setVisible(false);
    ui->actionLogout->setVisible(true);
    ui->actionChange_password->setVisible(true);
}

void C5MainWindow::updateTimeout()
{
    fUpdateTimer.stop();
    CheckForUpdateThread *ut = new CheckForUpdateThread(this);
    ut->fApplication = _MODULE_;
    ut->fDbParams = C5Config::dbParams();
    connect(ut, SIGNAL(checked(bool, int, QString)), this, SLOT(updateChecked(bool, int, QString)));
    ut->start();
}

void C5MainWindow::updateChecked(bool needUpdate, int source, const QString &path)
{
    if (!needUpdate) {
        fUpdateTimer.start(60000);
        return;
    }
    if (path.isEmpty()) {
        fToolbarWidget->setUpdateButtonVisible(true);
        C5Message::info(tr("Update exists, but you will update manually"));
        fUpdateTimer.start(60000);
        return;
    }
    fToolbarWidget->setUpdateButtonVisible(true);
    switch (source) {
    case usNone:
        fUpdateTimer.start(60000);
        break;
    case usLocalnet: {
        QStringList args;
        args << "-lhttp://" + C5Config::fDBHost;
        args << "-s0";
        QProcess p;
        p.start(qApp->applicationDirPath() + "/updater.exe", args);
        break;
    }
    }
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
    while (root->parent() != nullptr) {
        root = root->parent();
    }
    QString db = root->data(0, Qt::UserRole + 2).toString();
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

QStringList C5MainWindow::getDbParams(QTreeWidgetItem *item)
{
    QStringList dbParams;
    dbParams << item->data(0, Qt::UserRole + 1).toString()
             << item->data(0, Qt::UserRole + 2).toString()
             << item->data(0, Qt::UserRole + 3).toString()
             << item->data(0, Qt::UserRole + 4).toString()
             << item->data(0, Qt::UserRole).toString();
    return dbParams;
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
    QStringList dbParams = getDbParams(root);
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
    case cp_t2_calculate_self_cost: {
        createTab<C5DishSelfCostGenPrice>(dbParams);
        break;
    }
    case cp_t2_store_inventory:
        createTab<C5StoreInventory>(dbParams);
        break;
    case cp_t2_count_output_of_sale:
        createTab<CR5ConsumptionBySales>(dbParams);
        break;
    case cp_t2_store_complectation: {
        C5StoreDoc *sd = createTab<C5StoreDoc>(dbParams);
        sd->setMode(C5StoreDoc::sdComplectation);
        break;
    }
    case cp_t3_sales_common:
        createTab<CR5CommonSales>(dbParams);
        break;
    case cp_t3_documents:
        createTab<CR5Documents>(dbParams);
        break;
    case cp_t3_documents_store:
        createTab<CR5StoreDocuments>(dbParams);
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
    case cp_t3_store_sale:
        createTab<CR5SaleFromStore>(dbParams);
        break;
    case cp_t3_sale_removed_dishes:
        createTab<CR5SaleRemovedDishes>(dbParams);
        break;
    case cp_t3_sale_dishes:
        createTab<CR5SalesByDishes>(dbParams);
        break;
    case cp_t3_sale_from_store_total:
        createTab<CR5SaleFromStoreTotal>(dbParams);
        break;
    case cp_t3_discount_statistics:
        createTab<CR5DiscountStatisics>(dbParams);
        break;
    case cp_t3_debts_to_partners:
        createTab<CR5DebtsToPartner>(dbParams);
        break;
    case cp_t3_consuption_reason:
        createTab<CR5ConsuptionReason>(dbParams);
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
    case cp_t4_dishes_packages:
        createTab<CR5DishPackage>(dbParams);
        break;
    case cp_t4_menu_names:
        createTab<CR5MenuNames>(dbParams);
        break;
    case cp_t4_dish_remove_reason:
        createTab<CR5DishRemoveReason>(dbParams);
        break;
    case cp_t4_dish_comments:
        createTab<CR5DishComment>(dbParams);
        break;
    case cp_t4_dish_price_self_cost:
        createTab<CR5DishPriceSelfCost>(dbParams);
        break;
    case cp_t4_menu_review:
        createTab<CR5MenuReview>(dbParams);
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
    case cp_t6_classes:
        createTab<CR5GoodsClasses>(dbParams);
        break;
    case cp_t7_credit_card:
        createTab<CR5CreditCards>(dbParams);
        break;
    case cp_t7_customers:
        createTab<CR5Customers>(dbParams);
        break;
    case cp_t7_discount_system:
        createTab<CR5DiscountSystem>(dbParams);
        break;
    case cp_t7_halls:
        createTab<CR5Hall>(dbParams);
        break;
    case cp_t7_tables:
        createTab<CR5Tables>(dbParams);
        break;
    case cp_t7_translator:
        createTab<C5TranslatorForm>(dbParams);
        break;
    case cp_t7_store_reason:
        createTab<CR5StoreReason>(dbParams);
        break;
    case cp_t8_cash_names:
        createTab<CR5CashNames>(dbParams);
        break;
    case cp_t8_cash_doc:
        createTab<C5CashDoc>(dbParams);
        break;
    case cp_t8_costumer_debts_pay: {
        auto *cp = new C5CostumerDebtPayment(dbParams);
        cp->exec();
        delete cp;
        break;
    }
    case cp_t8_cash_detailed_report:
        createTab<CR5CashDetailed>(dbParams);
        break;
    case cp_t8_costumer_debts:
        createTab<CR5CostumerDebts>(dbParams);
        break;
    case cp_t9_salary_doc:
        createTab<C5SalaryDoc>(dbParams);
        break;
    case cp_t9_report:
        createTab<CR5SalaryByWorkers>(dbParams);
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
    C5WelcomePage *wp = nullptr;
    if (isVisible) {
        wp = dynamic_cast<C5WelcomePage*>(fTab->widget(0));
    }
    isVisible = wp != nullptr;
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

void C5MainWindow::on_twDb_itemExpanded(QTreeWidgetItem *item)
{
    if (item->parent()) {
        return;
    }
    for (int i = 0; i < ui->twDb->topLevelItemCount(); i++) {
        if (ui->twDb->topLevelItem(i) != item) {
            ui->twDb->collapseItem(ui->twDb->topLevelItem(i));
        }
    }
}

void C5MainWindow::on_actionLogout_triggered()
{
    for (int i = ui->tabWidget->count() - 1; i > -1 ; i--) {
        QWidget *w = ui->tabWidget->widget(i);
        w->deleteLater();
        ui->tabWidget->removeTab(i);
    }
    ui->twDb->clear();
    ui->actionLogin->setVisible(true);
    ui->actionLogout->setVisible(false);
    ui->actionChange_password->setVisible(false);
}

void C5MainWindow::on_actionChange_password_triggered()
{
    if (ui->twDb->topLevelItemCount() == 0) {
        return;
    }
    QString password;
    if (!C5ChangePassword::changePassword(getDbParams(ui->twDb->topLevelItem(0)), password)) {
        return;
    }
    for (int i = 0; i < ui->twDb->topLevelItemCount(); i++) {
        C5Database db(getDbParams(ui->twDb->topLevelItem(i)));
        db[":f_id"] = __userid;
        db[":f_password"] = password;
        db.exec("update s_user set f_password=md5(:f_password) where f_id=:f_id");
    }
    C5Message::info(tr("Password changed"));
}

void C5MainWindow::on_splitter_splitterMoved(int pos, int index)
{
    Q_UNUSED(pos);
    Q_UNUSED(index);
    C5Config::setRegValue("twdbsize", ui->twDb->width());
}
