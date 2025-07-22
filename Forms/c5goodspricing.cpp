#include "c5goodspricing.h"
#include "ui_c5goodspricing.h"

C5GoodsPricing::C5GoodsPricing() :
    C5Dialog(),
    ui(new Ui::C5GoodsPricing)
{
    ui->setupUi(this);
}

C5GoodsPricing::~C5GoodsPricing()
{
    delete ui;
}

int C5GoodsPricing::roundValue()
{
    return ui->leRoundValue->getInteger();
}

void C5GoodsPricing::on_btnCancel_clicked()
{
    reject();
}

void C5GoodsPricing::on_btnOK_clicked()
{
    accept();
}
