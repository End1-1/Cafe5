#include "wdashboard.h"
#include <QDialog>
#include "C5StoreInput.h"
#include "c5mainwindow.h"
#include "c5storeinventory.h"
#include "c5storeoutput.h"
#include "cr5goods.h"
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

    Q_ASSERT_X(false, "check name", QString("NO WIDGET NAMED %1 ").arg(name).toLatin1());
    return nullptr;
}
