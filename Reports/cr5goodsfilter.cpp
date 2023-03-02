#include "cr5goodsfilter.h"
#include "ui_cr5goodsfilter.h"
#include "c5cache.h"

CR5GoodsFilter::CR5GoodsFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5GoodsFilter)
{
    ui->setupUi(this);
    ui->leSupplier->setSelector(dbParams, ui->leSupplierName, cache_goods_partners);
    ui->leGroup->setSelector(dbParams, ui->leGroupName, cache_goods_group);
    ui->leClass1->setSelector(dbParams, ui->leClass1Name, cache_goods_classes);
    ui->leClass2->setSelector(dbParams, ui->leClass2Name, cache_goods_classes);
    ui->leClass3->setSelector(dbParams, ui->leClass3Name, cache_goods_classes);
    ui->leClass4->setSelector(dbParams, ui->leClass4Name, cache_goods_classes);
    ui->leCurr->setSelector(dbParams, ui->leCurrName, cache_currency);
}

CR5GoodsFilter::~CR5GoodsFilter()
{
    delete ui;
}

QString CR5GoodsFilter::condition()
{
    QString w;
    if (!ui->leSupplier->isEmpty()) {
        addCond(w, "gg.f_supplier in  (" + ui->leSupplier->text() + ") ");
    }
    if (!ui->leGroup->isEmpty()) {
        addCond(w, "gg.f_group in (" + ui->leGroup->text() + ") ");
    }
    if (!ui->leClass1->isEmpty()) {
        addCond(w, "gg.f_group1 in (" + ui->leClass1->text() + ") ");
    }
    if (!ui->leClass2->isEmpty()) {
        addCond(w, "gg.f_group2 in (" + ui->leClass2->text() + ") ");
    }
    if (!ui->leClass3->isEmpty()) {
        addCond(w, "gg.f_group3 in (" + ui->leClass3->text() + ") ");
    }
    if (!ui->leClass4->isEmpty()) {
        addCond(w, "gg.f_group4 in (" + ui->leClass4->text() + ") ");
    }
    if (ui->rbService->isChecked()) {
        addCond(w, "gg.f_service=1 ");
    }
    if (ui->rbNoService->isChecked()) {
        addCond(w, "gg.f_service=0 ");
    }
    if (ui->rbActiveYes->isChecked()) {
        addCond(w, "gg.f_enabled=1 ");
    }
    if (ui->rbActiveNo->isChecked()) {
        addCond(w, "gg.f_enabled=0 ");
    }
    in(w, "gpr.f_currency", ui->leCurr);
    if (!w.isEmpty()) {
        " where " + w;
    }
    return w;
}

int CR5GoodsFilter::currency()
{
    return ui->leCurr->getInteger();
}

QString CR5GoodsFilter::group()
{
    return ui->leGroup->text();
}

void CR5GoodsFilter::addCond(QString &w, const QString &cond)
{
    if (w.isEmpty()) {
        w += cond;
    } else {
        w += " and " + cond;
    }
}
