#include "dlgguests.h"
#include "ui_dlgguests.h"

DlgGuests::DlgGuests(C5User *user)
    : C5Dialog(user)
    , ui(new Ui::DlgGuests)
{
    ui->setupUi(this);
}

DlgGuests::~DlgGuests()
{
    delete ui;
}

void DlgGuests::setGuests(int value)
{
    ui->spGuest->setValue(value);
}

int DlgGuests::guests()
{
    return ui->spGuest->value();
}

void DlgGuests::on_btnGuestUp_clicked()
{
    ui->spGuest->setValue(ui->spGuest->value() + 1);
}

void DlgGuests::on_btnGuestDown_clicked()
{
    if(ui->spGuest->value() < 2) {
        return;
    }

    ui->spGuest->setValue(ui->spGuest->value() - 1);
}

void DlgGuests::on_btnAccept_clicked()
{
    accept();
}

void DlgGuests::on_btnCancel_clicked()
{
    reject();
}
