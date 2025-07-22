#include "cr5materialinstoreuncomplectfilter.h"
#include "ui_cr5materialinstoreuncomplectfilter.h"
#include "c5cache.h"

CR5MaterialInStoreUncomplectFilter::CR5MaterialInStoreUncomplectFilter(QWidget *parent) :
    C5FilterWidget(parent),
    ui(new Ui::CR5MaterialInStoreUncomplectFilter)
{
    ui->setupUi(this);
    ui->leStore->setSelector(ui->leStoreName, cache_goods_store).setMultiselection(true);
    ui->leGroup->setSelector(ui->leGroupname, cache_goods_group).setMultiselection(true);
    ui->leMaterial->setSelector(ui->leMaterialName, cache_goods, 1, 3).setMultiselection(true);
    ui->leUnit->setSelector(ui->leUnitName, cache_goods_unit).setMultiselection(true);
}

CR5MaterialInStoreUncomplectFilter::~CR5MaterialInStoreUncomplectFilter()
{
    delete ui;
}

QString CR5MaterialInStoreUncomplectFilter::condition()
{
    return "";
}

const QDate CR5MaterialInStoreUncomplectFilter::date() const
{
    return ui->deDate->date();
}

const QString CR5MaterialInStoreUncomplectFilter::unit() const
{
    return ui->leUnit->text();
}

const QString CR5MaterialInStoreUncomplectFilter::store() const
{
    return ui->leStore->text();
}

const QString CR5MaterialInStoreUncomplectFilter::goods() const
{
    return ui->leMaterial->text();
}

const QString CR5MaterialInStoreUncomplectFilter::group() const
{
    return ui->leGroup->text();
}

int CR5MaterialInStoreUncomplectFilter::viewMode()
{
    if (ui->rbGoods->isChecked()) {
        return 0;
    }
    if (ui->rbGroups->isChecked()) {
        return 1;
    }
    return 0;
}
