#include "cr5materialinstorefilter.h"
#include "ui_cr5materialinstorefilter.h"
#include "c5cache.h"

CR5MaterialInStoreFilter::CR5MaterialInStoreFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5MaterialInStoreFilter)
{
    ui->setupUi(this);
    ui->leStore->setSelector(fDBParams, ui->leStoreName, cache_goods_store);
    ui->leMaterial->setSelector(fDBParams, ui->leMaterialName, cache_goods, 0, 2);
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
    if (!ui->leMaterial->isEmpty()) {
        cond += " and s.f_goods in (" + ui->leMaterial->text() + ") ";
    }
    return cond;
}
