#include "ce5goodsgroup.h"
#include "ui_ce5goodsgroup.h"
#include "c5database.h"
#include "c5message.h"

CE5GoodsGroup::CE5GoodsGroup(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5GoodsGroup)
{
    ui->setupUi(this);
}

CE5GoodsGroup::~CE5GoodsGroup()
{
    delete ui;
}

void CE5GoodsGroup::on_btnSetStoreReminderQty_clicked()
{
    C5Database db(fDBParams);
    db[":f_group"] = ui->leCode->getInteger();
    db[":f_lowlevel"] = ui->leLowLevel->getInteger();
    db.exec("update c_goods set f_lowlevel=:f_lowlevel where f_group=:f_group");
    C5Message::info(tr("Done"));
}
