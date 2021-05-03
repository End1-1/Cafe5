#include "dlgguests.h"
#include "ui_dlgguests.h"
#include "c5waiterorderdoc.h"
#include "worder.h"
#include "wguestorder.h"

DlgGuests::DlgGuests(const QStringList &dbParams, C5WaiterOrderDoc *order) :
    C5Dialog(dbParams),
    fOrder(order),
    ui(new Ui::DlgGuests)
{
    ui->setupUi(this);
    ui->lst->setFlow(QListWidget::LeftToRight);
    ui->spGuest->setValue(fOrder->hInt("f_guests"));
    showMaximized();
    hide();
    if (fOrder->hInt("f_splitted") == 1) {
        on_btnSplitOrder_clicked();
    }
}

DlgGuests::~DlgGuests()
{
    delete ui;
}

void DlgGuests::on_btnGuestUp_clicked()
{
    ui->spGuest->setValue(ui->spGuest->value() + 1);
    fOrder->hSetInt("f_guests", ui->spGuest->value());
}

void DlgGuests::on_btnGuestDown_clicked()
{
    ui->spGuest->setValue(ui->spGuest->value() - 1);
    fOrder->hSetInt("f_guests", ui->spGuest->value());
}

void DlgGuests::on_btnSplitOrder_clicked()
{
    for (int i = ui->lst->count(); i < ui->spGuest->value(); i++) {
        auto *o = new WGuestOrder();
        QListWidgetItem *item = new QListWidgetItem(ui->lst);
        item->setSizeHint(QSize(300, 1000));
        ui->lst->addItem(item);
        ui->lst->setItemWidget(item, o);
        o->worder()->config(fOrder);
        o->worder()->itemsToTable(i + 1);
    }
    fOrder->hSetInt("f_splitted", 1);
}
