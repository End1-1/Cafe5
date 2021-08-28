#include "cr5tstoreextrafilter.h"
#include "ui_cr5tstoreextrafilter.h"
#include "c5cache.h"

CR5TStoreExtraFilter::CR5TStoreExtraFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5TStoreExtraFilter)
{
    ui->setupUi(this);
    ui->leStore->setSelector(fDBParams, ui->leStoreName, cache_goods_store);
}

CR5TStoreExtraFilter::~CR5TStoreExtraFilter()
{
    delete ui;
}

QString CR5TStoreExtraFilter::condition()
{
    return "";
}

QDate CR5TStoreExtraFilter::dateStart()
{
    return ui->deStart->date();
}

QDate CR5TStoreExtraFilter::dateEnd()
{
    return ui->deEnd->date();
}

int CR5TStoreExtraFilter::store()
{
    return ui->leStore->getInteger();
}

int CR5TStoreExtraFilter::pricing()
{
    if (ui->rbStorePrice->isChecked()) {
        return 1;
    } else if (ui->rbGoodsPrice->isChecked()) {
        return 2;
    }
    return 0;
}
