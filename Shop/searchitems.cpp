#include "searchitems.h"
#include "ui_searchitems.h"
#include "c5database.h"
#include "c5config.h"
#include "dlgreservgoods.h"
#include "datadriver.h"
#include "goodsreserve.h"
#include <QDebug>

SearchItems::SearchItems() :
    C5Dialog(),
    ui(new Ui::SearchItems)
{
    ui->setupUi(this);
    showMaximized();
    ui->tblReserve->setVisible(false);
}

SearchItems::~SearchItems()
{
    delete ui;
}

void SearchItems::on_btnSearch_clicked()
{
    ui->tblReserve->setVisible(false);
    ui->tblReserve->clearContents();
    ui->tbl->setVisible(true);
    ui->tbl->setRowCount(0);
    ui->leTotalQty->setDouble(0);

    if(ui->leCode->isEmpty()) {
        return;
    }

    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 200, 200, 200, 80, 100, 100, 100, 100, 0, 0);
    ui->tbl->setColumnHidden(4, __c5config.shopDenyF1());
    ui->tbl->setColumnHidden(5, __c5config.shopDenyF2());
    C5Database db;
    int goods = dbgoods->idOfScancode(ui->leCode->text());
    db[":f_goods"] = goods;
    db[":f_store"] = __c5config.defaultStore();
    db[":f_reservestate"] = GR_RESERVED;
    db.exec("select ss.f_name as f_storename,g.f_name as f_goodname,g.f_scancode, "
            "u.f_name as f_unit,"
            "if(gpr.f_price1disc>0,gpr.f_price1disc, gpr.f_price1) as f_price1,"
            "gpr.f_price2, sum(s.f_qty*s.f_type) as f_qty, "
            "coalesce(rs.f_qty, 0) as f_reserveqty, s.f_store, s.f_goods "
            "from a_store s "
            "inner join c_storages ss on ss.f_id=s.f_store "
            "inner join c_goods g on g.f_id=s.f_goods "
            "inner join c_units u on u.f_id=g.f_unit "
            "inner join a_header h on h.f_id=s.f_document  "
            "left join c_goods_prices gpr on gpr.f_goods=g.f_id "
            "left join (select f_goods, f_store, sum(f_qty) as f_qty "
            "from a_store_reserve where f_state=:f_reservestate and f_goods=:f_goods group by 1, 2) rs on rs.f_goods=s.f_goods and rs.f_store=s.f_store "
            "where s.f_goods=:f_goods "
            "group by ss.f_name,g.f_name,u.f_name,gpr.f_price1,gpr.f_price2 "
            "having sum(s.f_qty*s.f_type) > 0 "
            "union "
            "select '-', g.f_name as f_goodname,g.f_scancode, "
            "u.f_name as f_unit,if(gpr.f_price1disc>0,gpr.f_price1disc,gpr.f_price1) as f_price1, "
            "gpr.f_price2, 99999 as f_qty, "
            "coalesce(rs.f_qty, 0) as f_reserveqty, 1, g.f_id as f_goods "
            "from c_goods g "
            "inner join c_units u on u.f_id=g.f_unit "
            "left join c_goods_prices gpr on gpr.f_goods=g.f_id "
            "left join (select f_goods, f_store, sum(f_qty) as f_qty "
            "from a_store_reserve where f_state=:f_reservestate "
            "and f_goods=:f_goods group by 1, 2) "
            "rs on rs.f_goods=g.f_id and rs.f_store=1 "
            "where g.f_id=:f_goods and g.f_service=1 "
           );

    while(db.nextRow()) {
        int r = ui->tbl->addEmptyRow();

        for(int i = 0; i < db.columnCount(); i++) {
            ui->tbl->setData(r, i, db.getValue(i));
        }

        ui->tbl->setDouble(r, 4, db.getDouble("f_price1"));
        ui->tbl->setDouble(r, 5, db.getDouble("f_price2"));
        ui->tbl->setDouble(r, 6, db.getDouble("f_qty"));
        ui->tbl->setDouble(r, 7, db.getDouble("f_reserveqty"));
        ui->leTotalQty->setDouble(ui->leTotalQty->getDouble() + db.getDouble("f_qty"));
    }

    ui->leCode->clear();
}

void SearchItems::on_leCode_returnPressed()
{
    on_btnSearch_clicked();
}

void SearchItems::on_btnReserve_clicked()
{
    int row = ui->tbl->currentRow();

    if(row < 0) {
        return;
    }

    DlgReservGoods r(
        ui->tbl->getInteger(row, tr("Store code")),
        ui->tbl->getInteger(row, tr("Goods code")),
        ui->tbl->getDouble(row, tr("Qty")) - ui->tbl->getDouble(row, tr("Reserved")));
    r.exec();
}

void SearchItems::on_btnViewReservations_clicked()
{
    int goods = 0;
    int store = 0;
    QString where;

    if(ui->tbl->currentRow() > -1) {
        goods = ui->tbl->getInteger(ui->tbl->currentRow(), 9);
        store = ui->tbl->getInteger(ui->tbl->currentRow(), 8);
        where = QString(" and rs.f_goods=%1 and rs.f_store=%2 ")
                .arg(goods)
                .arg(store);
    }

    ui->tblReserve->setRowCount(0);
    C5Database db;
    db[":f_reservestate"] = GR_RESERVED;
    db.exec("select rs.f_id, arn.f_name as f_statename, rs.f_date, "
            "ss.f_name as f_sourcestore, sd.f_name as f_storename, "
            "g.f_name as f_goodsname, g.f_scancode, "
            "rs.f_qty, rs.f_message "
            "from a_store_reserve rs "
            "left join c_storages ss on ss.f_id=rs.f_source "
            "left join c_storages sd on sd.f_id=rs.f_store "
            "left join c_goods g on g.f_id=rs.f_goods "
            "left join a_store_reserve_state arn on arn.f_id=rs.f_state "
            "where rs.f_state=:f_reservestate " + where +
            "order by rs.f_date ");

    while(db.nextRow()) {
        int r = ui->tblReserve->addEmptyRow();

        for(int i = 0; i < ui->tblReserve->columnCount(); i++) {
            ui->tblReserve->setData(r, i, db.getValue(i));
        }

        ui->tbl->setDouble(r, 6, db.getDouble("f_qty"));
    }

    ui->tbl->setVisible(false);
    ui->tbl->clearContents();
    ui->tblReserve->setColumnWidths(ui->tblReserve->columnCount(), 0, 120, 120, 200, 200, 200, 120, 80, 200);
    ui->tblReserve->setVisible(true);
}

void SearchItems::on_btnViewAllReservations_clicked()
{
    ui->tblReserve->setRowCount(0);
    C5Database db;
    db.exec("select rs.f_id, arn.f_name as f_statename, rs.f_date, "
            "ss.f_name as f_sourcestore, sd.f_name as f_storename, "
            "g.f_name as f_goodsname, g.f_scancode, "
            "rs.f_qty, rs.f_message "
            "from a_store_reserve rs "
            "left join c_storages ss on ss.f_id=rs.f_source "
            "left join c_storages sd on sd.f_id=rs.f_store "
            "left join c_goods g on g.f_id=rs.f_goods "
            "left join a_store_reserve_state arn on arn.f_id=rs.f_state "
            "order by rs.f_date ");

    while(db.nextRow()) {
        int r = ui->tblReserve->addEmptyRow();

        for(int i = 0; i < ui->tblReserve->columnCount(); i++) {
            ui->tblReserve->setData(r, i, db.getValue(i));
        }

        ui->tblReserve->setDouble(r, 7, db.getDouble("f_qty"));
    }

    ui->tbl->setVisible(false);
    ui->tbl->clearContents();
    ui->tblReserve->setColumnWidths(ui->tblReserve->columnCount(), 0, 120, 120, 200, 200, 200, 120, 80, 200);
    ui->tblReserve->setVisible(true);
}

void SearchItems::on_btnEditReserve_clicked()
{
    if(!ui->tblReserve->isVisible()) {
        ui->btnViewReservations->click();
        return;
    }

    int r = ui->tblReserve->currentRow();
    int reserveid = 0;

    if(r > -1) {
        reserveid = ui->tblReserve->getInteger(r, "id");
    } else {
        return;
    }

    DlgReservGoods rg(reserveid);
    rg.exec();
}

void SearchItems::on_btnExit_clicked()
{
    reject();
}
