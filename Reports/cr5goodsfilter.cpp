#include "cr5goodsfilter.h"
#include "ui_cr5goodsfilter.h"
#include "c5cache.h"

CR5GoodsFilter::CR5GoodsFilter(QWidget *parent) :
    C5FilterWidget(parent),
    ui(new Ui::CR5GoodsFilter)
{
    ui->setupUi(this);
    ui->leSupplier->setSelector(ui->leSupplierName, cache_goods_partners);
    ui->leGroup->setSelector(ui->leGroupName, cache_goods_group);
    ui->leCurr->setSelector(ui->leCurrName, cache_currency);
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
