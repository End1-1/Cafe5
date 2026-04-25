#include "wdashboard.h"
#include <QDialog>
#include "C5StoreInput.h"
#include "c5config.h"
#include "c5mainwindow.h"
#include "c5storeinventory.h"
#include "c5storeoutput.h"
#include "cr5goods.h"
#include "cr5goodsgroup.h"
#include "cr5goodspartners.h"
#include "cr5goodsunit.h"
#include "cr5hall.h"
#include "cr5tables.h"
#include "cr5users.h"
#include "cr5usersgroups.h"
#include "ntablewidget.h"
#include "rabstracteditorreport.h"
#include "ui_wdashboard.h"

WDashboard::WDashboard(C5User *user, QWidget *parent)
    : C5Widget(parent)
    , ui(new Ui::WDashboard)
{
    ui->setupUi(this);
    mUser = user;
    fLabel = tr("Dashboard");
    auto buttons = findChildren<QAbstractButton*>();

    for(auto *btn : std::as_const(buttons)) {
        if(btn->property("form").isValid()) {
            connect(btn, &QAbstractButton::clicked, this, &WDashboard::onCommandButtonClicked);
        }
    }
}

WDashboard::~WDashboard() { delete ui; }

void WDashboard::onCommandButtonClicked()
{
    auto *btn = qobject_cast<QAbstractButton*>(sender());

    if(!btn)
        return;

    QString form = btn->property("form").toString();
    auto *w = createForm(form, btn->icon());
    auto *dialog = qobject_cast<QDialog*>(w);

    if(dialog) {
        dialog->exec();
        return;
    }

    auto *c5widget = qobject_cast<C5Widget*>(w);

    if(c5widget) {
        __mainWindow->addWidget(c5widget);
        return;
    }
}

QWidget* WDashboard::createForm(const QString &name, QIcon icon)
{
    if (name == "form_workstations") {
        return new RAbstractEditorReport(tr("Workstations"), icon, name);
    }

    if (name == "form_cashsessions") {
        return new RAbstractEditorReport(tr("Cash sessions"), icon, name);
    }

    if (name == "form_newstoreinput") {
        return new C5StoreInput(mUser, tr("Store input"), icon);
    }

    if (name == "form_newstoreoutput") {
        return new C5StoreOutput(mUser, tr("Store output"), icon);
    }

    if (name == "form_store_documents") {
        return new RAbstractEditorReport(tr("Store documents"), icon, name);
    }

    if (name == "form_store_moves") {
        return new RAbstractEditorReport(tr("Store moves"), icon, name);
    }

    if (name == "form_stock") {
        return new RAbstractEditorReport(tr("Stock"), icon, name);
    }

    if (name == "form_inventory") {
        auto *a = new C5StoreInventory(mUser, tr("Store inventory"), icon);
        return a;
    }

    if (name == "form_goods") {
        auto *a = __mainWindow->createTab<CR5Goods>();
        return a;
    }

    if (name == "form_inventory") {
        auto *a = __mainWindow->createTab<CR5Goods>();
        return a;
    }

    if (name == "form_inventory_documents") {
        return new RAbstractEditorReport(tr("Inventory"), icon, name);
    }

    if (name == "form_inventory_documents") {
        return new RAbstractEditorReport(tr("Inventory"), icon, name);
    }

    if (name == "form_sold_items") {
        return new RAbstractEditorReport(tr("Sold items"), icon, name);
    }

    if (name == "form_consumed_goods") {
        return new RAbstractEditorReport(tr("Consumed goods"), icon, name);
    }

    if (name == "form_revenue") {
        return new RAbstractEditorReport(tr("Revenue"), icon, name);
    }

    if (name == "form_groups_of_goods") {
        auto *gr = new CR5GoodsGroup();
        gr->postProcess();
        return gr;
    }

    if (name == "form_goods_units") {
        auto *gu = new CR5GoodsUnit();
        gu->postProcess();
        return gu;
    }

    if (name == "form_job_title") {
        auto *jobTitles = new CR5UsersGroups();
        jobTitles->postProcess();
        return jobTitles;
    }

    if (name == "form_staff") {
        auto *users = new CR5Users();
        users->postProcess();
        return users;
    }

    if (name == "form_halls") {
        auto *halls = new CR5Hall();
        halls->postProcess();
        return halls;
    }

    if (name == "form_tables") {
        auto *tables = new CR5Tables();
        tables->postProcess();
        return tables;
    }

    if (name == "form_city_ledger") {
        auto *t = new NTableWidget(mUser, "/engine/reports/customer-debts.php");
        t->mMainWindow = __mainWindow;
        t->mHost = __c5config.dbParams().at(1);
        t->initParams({});
        t->query();
        return t;
    }

    if (name == "form_debts") {
        return new RAbstractEditorReport(tr("City ledger"), icon, name);
    }

    if (name == "form_partners") {
        auto *partners = new CR5GoodsPartners();
        partners->postProcess();
        return partners;
    }

    if (name == "form_summary") {
        return new RAbstractEditorReport(tr("Store documents"), icon, name);
    }

    Q_ASSERT_X(false, "check name", QString("NO WIDGET NAMED %1 ").arg(name).toLatin1());
    return nullptr;
}
