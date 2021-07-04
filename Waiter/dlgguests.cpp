#include "dlgguests.h"
#include "ui_dlgguests.h"
#include "c5orderdriver.h"
#include "worder.h"
#include "wguestorder.h"

DlgGuests::DlgGuests(const QStringList &dbParams, C5OrderDriver *order) :
    C5Dialog(dbParams),
    fOrder(order),
    ui(new Ui::DlgGuests)
{

}

DlgGuests::~DlgGuests()
{
    delete ui;
}

void DlgGuests::on_btnGuestUp_clicked()
{
    ui->spGuest->setValue(ui->spGuest->value() + 1);
    fOrder->setHeaderOption("f_guests", ui->spGuest->value());
}

void DlgGuests::on_btnGuestDown_clicked()
{
    ui->spGuest->setValue(ui->spGuest->value() - 1);
    fOrder->setHeaderOption("f_guests", ui->spGuest->value());
}

void DlgGuests::on_btnSplitOrder_clicked()
{

}
