#include "c5dbresetoption.h"
#include "ui_c5dbresetoption.h"
#include "c5random.h"

C5DbResetOption::C5DbResetOption(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::C5DbResetOption)
{
    ui->setupUi(this);
    ui->lbSale->setText(C5Random::randomStr(1000, 9999));
    ui->lbFull->setText(C5Random::randomStr(1000, 9999));
}

C5DbResetOption::~C5DbResetOption()
{
    delete ui;
}

bool C5DbResetOption::saleAndBuy()
{
    return ui->chSale->isChecked();
}

bool C5DbResetOption::fullReset()
{
    return ui->chFull->isChecked();
}

void C5DbResetOption::on_btnCancel_clicked()
{
    reject();
}

void C5DbResetOption::on_btnAccept_clicked()
{
    if (!ui->chSale->isChecked() && !ui->chFull->isChecked()) {
        C5Message::error(tr("Select atleast one option"));
        return;
    }
    if (ui->chSale->isChecked()) {
        if (ui->lbSale->text() != ui->leSale->text()) {
            C5Message::error(tr("Checking code for sales and buy not correct"));
            return;
        }
    }
    if (ui->chFull->isChecked()) {
        if (ui->lbFull->text() != ui->leFull->text()) {
            C5Message::error(tr("Checking code for full reset not correct"));
            return;
        }
    }
    accept();
}
