#include "c5changepriceofgroup.h"
#include "ui_c5changepriceofgroup.h"
#include "c5random.h"

C5ChangePriceOfGroup::C5ChangePriceOfGroup(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::C5ChangePriceOfGroup)
{
    ui->setupUi(this);
    ui->lbCheckcode->setText(C5Random::randomStr(1000, 9999));
}

C5ChangePriceOfGroup::~C5ChangePriceOfGroup()
{
    delete ui;
}

bool C5ChangePriceOfGroup::groupPrice(const QStringList &dbParams, double &price1, double &price2)
{
    auto *c = new  C5ChangePriceOfGroup(dbParams);
    bool r = c->exec() == QDialog::Accepted;
    price1 = c->ui->lePrice1->getDouble();
    price2 = c->ui->lePrice2->getDouble();
    return r;
}

void C5ChangePriceOfGroup::on_btnOk_clicked()
{
    if (ui->lbCheckcode->text() != ui->leCheckCode->text()) {
        C5Message::error(tr("Check code is not valid"));
        return;
    }
    accept();
}

void C5ChangePriceOfGroup::on_btnCancel_clicked()
{
    reject();
}
