#include "cr5salefromstorefilter.h"
#include "ui_cr5salefromstorefilter.h"
#include "c5cache.h"

CR5SaleFromStoreFilter::CR5SaleFromStoreFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5SaleFromStoreFilter)
{
    ui->setupUi(this);
    ui->leSupplier->setSelector(dbParams, ui->leSupplierName, cache_goods_partners);
    ui->leGroup->setSelector(dbParams, ui->leGroupName, cache_goods_group);
    ui->leClass1->setSelector(dbParams, ui->leClass1Name, cache_goods_classes);
    ui->leClass2->setSelector(dbParams, ui->leClass2Name, cache_goods_classes);
    ui->leClass3->setSelector(dbParams, ui->leClass3Name, cache_goods_classes);
    ui->leClass4->setSelector(dbParams, ui->leClass4Name, cache_goods_classes);
}

CR5SaleFromStoreFilter::~CR5SaleFromStoreFilter()
{
    delete ui;
}

QString CR5SaleFromStoreFilter::condition()
{
    QString w = " oh.f_datecash between " + ui->deStart->toMySQLDate() + " and " + ui->deEnd->toMySQLDate() + " ";
    if (!ui->leSupplier->isEmpty()) {
        w += " and gg.f_supplier in (" + ui->leSupplier->text() + ") " ;
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
