#include "selectprinters.h"
#include "ui_selectprinters.h"
#include "c5config.h"

SelectPrinters::SelectPrinters(C5User *user) :
    C5Dialog(user),
    ui(new Ui::SelectPrinters)
{
    ui->setupUi(this);
}

SelectPrinters::~SelectPrinters()
{
    delete ui;
}

bool SelectPrinters::selectPrinters(bool &p1, bool &p2, C5User *user)
{
    SelectPrinters sp(user);
    bool result = sp.exec() == QDialog::Accepted;
    p1 = sp.ui->chP1->isChecked();
    p2 = sp.ui->chP2->isChecked();
    return result;
}

void SelectPrinters::on_btnCancel_clicked()
{
    reject();
}

void SelectPrinters::on_btnOk_clicked()
{
    accept();
}
