#include "ce5goodswaste.h"
#include "ui_ce5goodswaste.h"
#include "c5cache.h"
#include "c5utils.h"

CE5GoodsWaste::CE5GoodsWaste(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5GoodsWaste)
{
    ui->setupUi(this);
    ui->leGoods->setSelector(dbParams, ui->leGoodsName, cache_goods, 1, 3);
    ui->leReason->setSelector(dbParams, ui->leReasonName, cache_store_reason);
    ui->leReason->setValue(DOC_REASON_TRASH);
}

CE5GoodsWaste::~CE5GoodsWaste()
{
    delete ui;
}
