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
    ui->leSupplier->setSelector(dbParams, ui->leSupplierName, cache_goods_partners).setMultiselection(true);
    ui->leGroup->setSelector(dbParams, ui->leGroupName, cache_goods_group).setMultiselection(true);
    ui->leClass1->setSelector(dbParams, ui->leClass1Name, cache_goods_classes).setMultiselection(true);
    ui->leClass2->setSelector(dbParams, ui->leClass2Name, cache_goods_classes).setMultiselection(true);
    ui->leClass3->setSelector(dbParams, ui->leClass3Name, cache_goods_classes).setMultiselection(true);
    ui->leClass4->setSelector(dbParams, ui->leClass4Name, cache_goods_classes).setMultiselection(true);
    ui->leBuyer->setSelector(dbParams, ui->leBuyerName, cache_goods_partners).setMultiselection(true);
}

CR5SaleFromStoreFilter::~CR5SaleFromStoreFilter()
{
    delete ui;
}

QString CR5SaleFromStoreFilter::condition()
{
    QString w = " oh.f_datecash between " + ui->deStart->toMySQLDate() + " and " + ui->deEnd->toMySQLDate() + " ";
    if (!ui->leSupplier->isEmpty()) {
        w += " and ah.f_partner in (" + ui->leSupplier->text() + ") " ;
    }
    if (!ui->leHall->isEmpty()) {
        w += " and oh.f_hall in (" + ui->leHall->text() + ") ";
    }
    if (!ui->leGroup->isEmpty()) {
        w += " and gg.f_group in (" + ui->leGroup->text() + ") ";
    }
    if (!ui->leClass1->isEmpty()) {
        w += " and gg.f_group1 in (" + ui->leClass1->text() + ") ";
    }
    if (!ui->leClass2->isEmpty()) {
        w += " and gg.f_group2 in (" + ui->leClass2->text() + ") ";
    }
    if (!ui->leClass3->isEmpty()) {
        w += " and gg.f_group3 in (" + ui->leClass3->text() + ") ";
    }
    if (!ui->leClass4->isEmpty()) {
        w += " and gg.f_group4 in (" + ui->leClass4->text() + ") ";
    }
    if (ui->rbFiscal->isChecked()) {
        w += " and og.f_tax>0 ";
    }
    if (ui->rbFiscalNone->isChecked()) {
        w += " and og.f_tax=0 ";
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
