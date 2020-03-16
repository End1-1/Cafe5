#include "cr5materialinstorefilter.h"
#include "ui_cr5materialinstorefilter.h"
#include "c5cache.h"

CR5MaterialInStoreFilter::CR5MaterialInStoreFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5MaterialInStoreFilter)
{
    ui->setupUi(this);
    ui->leStore->setSelector(fDBParams, ui->leStoreName, cache_goods_store);
    ui->leGroup->setSelector(fDBParams, ui->leGroupname, cache_goods_group);
    ui->leMaterial->setSelector(fDBParams, ui->leMaterialName, cache_goods, 1, 3);
    ui->leClass1->setSelector(fDBParams, ui->leClass1Name, cache_goods_classes);
    ui->leClass2->setSelector(fDBParams, ui->leClass2Name, cache_goods_classes);
    ui->leClass3->setSelector(fDBParams, ui->leClass3Name, cache_goods_classes);
    ui->leClass4->setSelector(fDBParams, ui->leClass4Name, cache_goods_classes);
}

CR5MaterialInStoreFilter::~CR5MaterialInStoreFilter()
{
    delete ui;
}

QString CR5MaterialInStoreFilter::condition()
{
    QString cond = QString(" h.f_date<=%1 ").arg(ui->deDate->toMySQLDate());
    if (!ui->leStore->isEmpty()) {
        cond += " and s.f_store in (" + ui->leStore->text() + ") ";
    }
    if (!ui->leGroup->isEmpty()) {
        cond += " and gg.f_id in (" + ui->leGroup->text() + ") ";
    }
    if (!ui->leMaterial->isEmpty()) {
        cond += " and s.f_goods in (" + ui->leMaterial->text() + ") ";
    }
    if (!ui->leClass1->isEmpty()) {
        cond += " and g.f_group1 in (" + ui->leClass1->text() + ") ";
    }
    if (!ui->leClass2->isEmpty()) {
        cond += " and g.f_group2 in (" + ui->leClass2->text() + ") ";
    }
    if (!ui->leClass3->isEmpty()) {
        cond += " and g.f_group3 in (" + ui->leClass3->text() + ") ";
    }
    if (!ui->leClass4->isEmpty()) {
        cond += " and g.f_group4 in (" + ui->leClass4->text() + ") ";
    }
    return cond;
}

bool CR5MaterialInStoreFilter::showDrafts()
{
    return ui->chShowDrafts->isChecked();
}
