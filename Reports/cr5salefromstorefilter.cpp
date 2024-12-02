#include "cr5salefromstorefilter.h"
#include "ui_cr5salefromstorefilter.h"
#include "c5cache.h"
#include "cr5saleflags.h"

CR5SaleFromStoreFilter::CR5SaleFromStoreFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5SaleFromStoreFilter)
{
    ui->setupUi(this);
    ui->leHall->setSelector(dbParams, ui->leHallname, cache_hall_list).setMultiselection(true);
    ui->leGroup->setSelector(dbParams, ui->leGroupName, cache_goods_group).setMultiselection(true);
    ui->leBuyer->setSelector(dbParams, ui->leBuyerName, cache_goods_partners).setMultiselection(true);
}

CR5SaleFromStoreFilter::~CR5SaleFromStoreFilter()
{
    delete ui;
}

QString CR5SaleFromStoreFilter::condition()
{
    QString w = " oh.f_datecash between " + ui->deStart->toMySQLDate() + " and " + ui->deEnd->toMySQLDate() + " ";
    if (!ui->leHall->isEmpty()) {
        w += " and oh.f_hall in (" + ui->leHall->text() + ") ";
    }
    if (!ui->leGroup->isEmpty()) {
        w += " and gg.f_group in (" + ui->leGroup->text() + ") ";
    }
    if (ui->rbFiscal->isChecked()) {
        w += " and coalesce(ot.f_receiptnumber, 0)>0 ";
    }
    if (ui->rbFiscalNone->isChecked()) {
        w += " and coalesce(ot.f_receiptnumber, 0)=0 ";
    }
    if (ui->rbRIYes->isChecked()) {
        w += " and oh.f_amounttotal<0 ";
    }
    if (ui->rbRINo->isChecked()) {
        w += " and oh.f_amounttotal>=0 ";
    }
    if (!ui->leBuyer->isEmpty()) {
        w += " and oh.f_partner in (" + ui->leBuyer->text() + ") ";
    }
    if (ui->rbSrvYes->isChecked()) {
        w += " and gg.f_service=1 ";
    }
    if (ui->rbSrvNo->isChecked()) {
        w += " and gg.f_service=0 ";
    }
    if (!ui->leClass->isEmpty()) {
        w += " and gr.f_class='" + ui->leClass->text() + "' ";
    }
    w += fFlags;
    return w;
}

QDate CR5SaleFromStoreFilter::d1()
{
    return ui->deStart->date();
}

QDate CR5SaleFromStoreFilter::d2()
{
    return ui->deEnd->date();
}

void CR5SaleFromStoreFilter::on_btnFlags_clicked()
{
    CR5SaleFlags f(fDBParams);
    if (f.exec() == QDialog::Accepted) {
        fFlags = f.flagsCond();
    }
}
