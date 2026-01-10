#include "dlgguests.h"
#include "ui_dlgguests.h"

DlgGuests::DlgGuests(C5User *user, C5OrderDriver *order) :
    C5Dialog(user),
    ui(new Ui::DlgGuests),
    fOrder(order)
{
    ui->setupUi(this);
//   ui->spGuest->setValue(order->headerOptionsValue("f_guests").toInt());
}

DlgGuests::~DlgGuests()
{
    delete ui;
}

void DlgGuests::on_btnGuestUp_clicked()
{
    ui->spGuest->setValue(ui->spGuest->value() + 1);
    // fOrder->setHeaderOption("f_guests", ui->spGuest->value());
}

void DlgGuests::on_btnGuestDown_clicked()
{
    if(ui->spGuest->value() < 2) {
        return;
    }

    ui->spGuest->setValue(ui->spGuest->value() - 1);
    //  fOrder->setHeaderOption("f_guests", ui->spGuest->value());
}

void DlgGuests::on_btnSplitOrder_clicked()
{
}
void DlgGuests::on_btnClose_clicked()
{
    accept();
}
