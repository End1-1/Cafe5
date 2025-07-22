#include "viewinputitem.h"
#include "ui_viewinputitem.h"
#include "c5database.h"
#include "c5config.h"

ViewInputItem::ViewInputItem(const QString &code, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewInputItem)
{
    ui->setupUi(this);
    C5Database db;
    db[":f_id"] = code;
    db.exec("select * from a_store_draft where f_id=:f_id");
    db.nextRow();
    int goodsId = db.getInt("f_goods");
    double qty = db.getDouble("f_qty");
    db[":f_base"] = goodsId;
    db.exec("select g.f_name, g.f_scancode, gpr.f_price1, gc.f_qty "
            "from c_goods_complectation gc "
            "inner join c_goods g on g.f_id=gc.f_goods "
            "left join c_goods_prices gpr on gpr.f_goods=g.f_id "
            "where gc.f_base=:f_base");
    while (db.nextRow()) {
        int r = ui->tbl->addEmptyRow();
        ui->tbl->setString(r, 0, "");
        ui->tbl->setString(r, 1, db.getString("f_name"));
        ui->tbl->setString(r, 2, db.getString("f_scancode"));
        ui->tbl->setDouble(r, 3, db.getDouble("f_qty") * qty);
        ui->tbl->setDouble(r, 4, db.getDouble("f_price1"));
    }
    if (ui->tbl->rowCount() == 0) {
        db[":f_id"] = goodsId;
        db.exec("select * from c_goods where f_id=:f_id");
        if (!db.nextRow()) {
            return;
        }
        int r = ui->tbl->addEmptyRow();
        ui->tbl->setString(r, 0, "");
        ui->tbl->setString(r, 1, db.getString("f_name"));
        ui->tbl->setString(r, 2, db.getString("f_scancode"));
        ui->tbl->setDouble(r, 3, qty);
        ui->tbl->setDouble(r, 4, db.getDouble("f_price1"));
    }
}

ViewInputItem::~ViewInputItem()
{
    delete ui;
}
