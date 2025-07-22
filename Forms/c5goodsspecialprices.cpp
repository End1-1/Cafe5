#include "c5goodsspecialprices.h"
#include "ui_c5goodsspecialprices.h"
#include "c5message.h"
#include "c5database.h"
#include "c5goodsspecialprice.h"
#include <QTableWidgetItem>

#define col_id 0
#define col_partner_id 1
#define col_partner_tin 2
#define col_partner_name 3
#define col_partner_address 4
#define col_goods_id 5
#define col_goods_group 6
#define col_goods_barcode 7
#define col_goods_name 8
#define col_goods_special_price 9
#define col_goods_retail 10
#define col_goods_whosale 11
#define col_goods_retail_diff 12
#define col_goods_whosale_diff 13

C5GoodsSpecialPrices::C5GoodsSpecialPrices(QWidget *parent) :
    C5Widget(parent),
    ui(new Ui::C5GoodsSpecialPrices)
{
    ui->setupUi(this);
    ui->tblp->setColumnWidths(ui->tblp->columnCount(), 0, 0, 100, 200, 300, 0, 100, 100, 200, 80, 80, 80, 80, 80);
    getPrices();
}

C5GoodsSpecialPrices::~C5GoodsSpecialPrices()
{
    delete ui;
}

QToolBar *C5GoodsSpecialPrices::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbSave;
        fToolBar = createStandartToolbar(btn);
    }
    return fToolBar;
}

void C5GoodsSpecialPrices::getPrices()
{
    ui->tblp->clearContents();
    ui->tblp->setRowCount(0);
    C5Database db;
    db.exec("select sp.f_partner as f_partnerid, p.f_taxcode, p.f_name as f_partnername, p.f_address, "
            "sp.f_id, sp.f_goods, gr.f_name as f_groupname, g.f_name as f_goodsname, gp.f_price1, gp.f_price2, sp.f_price, "
            "((sp.f_price * 100)/gp.f_price1)-100 as f_price1diff, ((sp.f_price * 100)/gp.f_price2)-100 as f_price2diff, "
            "g.f_scancode "
            "from c_goods_special_prices sp "
            "left join c_goods g on sp.f_goods=g.f_id  "
            "left join c_groups gr on gr.f_id=g.f_group "
            "left join c_goods_prices gp on gp.f_goods=g.f_id "
            "left join c_partners p on p.f_id=sp.f_partner "
            "where gp.f_currency=1 "
            "order by gr.f_name ");
    ui->tblp->setRowCount(db.rowCount());
    int r = 0;
    while (db.nextRow()) {
        ui->tblp->setInteger(r, col_id, db.getInt("f_id"));
        ui->tblp->setInteger(r, col_partner_id, db.getInt("f_partnerid"));
        ui->tblp->setString(r, col_partner_tin, db.getString("f_taxcode"));
        ui->tblp->setString(r, col_partner_name, db.getString("f_partnername"));
        ui->tblp->setString(r, col_partner_address, db.getString("f_address"));
        ui->tblp->setInteger(r, col_goods_id, db.getInt("f_goods"));
        ui->tblp->setString(r, col_goods_group, db.getString("f_groupname"));
        ui->tblp->setString(r, col_goods_barcode, db.getString("f_scancode"));
        ui->tblp->setString(r, col_goods_name, db.getString("f_goodsname"));
        ui->tblp->setDouble(r, col_goods_special_price, db.getDouble("f_price"));
        ui->tblp->setDouble(r, col_goods_retail, db.getDouble("f_price1"));
        ui->tblp->setDouble(r, col_goods_whosale, db.getDouble("f_price2"));
        ui->tblp->setDouble(r, col_goods_retail_diff, db.getDouble("f_price1diff"));
        ui->tblp->setDouble(r, col_goods_whosale_diff, db.getDouble("f_price2diff"));
        r++;
    }
}

void C5GoodsSpecialPrices::fillRow(C5GoodsSpecialPrice *sp, int row)
{
    if (row == -1) {
        row = ui->tblp->rowCount();
        ui->tblp->setRowCount(row + 1);
    }
    ui->tblp->setInteger(row, col_id, sp->getFieldIntValue("leID"));
    ui->tblp->setInteger(row, col_partner_id, sp->getFieldIntValue("lePartnerID"));
    ui->tblp->setString(row, col_partner_tin, sp->getFieldStringValue("lePartnerTIN"));
    ui->tblp->setString(row, col_partner_name, sp->getFieldStringValue("lePartnerName"));
    ui->tblp->setString(row, col_partner_address, sp->getFieldStringValue("lePartnerAddress"));
    ui->tblp->setInteger(row, col_goods_id, sp->getFieldIntValue("leGoodsID"));
    ui->tblp->setString(row, col_goods_group, sp->getFieldStringValue("leGoodsGroup"));
    ui->tblp->setString(row, col_goods_barcode, sp->getFieldStringValue("leBarcode"));
    ui->tblp->setString(row, col_goods_name, sp->getFieldStringValue("leGoodsName"));
    ui->tblp->setDouble(row, col_goods_special_price, sp->getFieldDoubleValue("leSpecialPrice"));
    ui->tblp->setDouble(row, col_goods_retail, sp->getFieldDoubleValue("leRetailPrice"));
    ui->tblp->setDouble(row, col_goods_whosale, sp->getFieldDoubleValue("leWhosalePrice"));
    ui->tblp->setDouble(row, col_goods_retail_diff, sp->getFieldDoubleValue("leRetailPriceDiff"));
    ui->tblp->setDouble(row, col_goods_whosale_diff, sp->getFieldDoubleValue("leWhosalePriceDiff"));
}

void C5GoodsSpecialPrices::on_tblp_itemClicked(QTableWidgetItem *item)
{
    int row = item->row();
    if (row < 0) {
        return;
    }
    C5GoodsSpecialPrice d;
    d.setFieldValue("leID", ui->tblp->getInteger(row, col_id));
    d.setFieldValue("lePartnerID", ui->tblp->getInteger(row, col_partner_id));
    d.setFieldValue("lePartnerTIN", ui->tblp->getInteger(row, col_partner_tin));
    d.setFieldValue("lePartnerName", ui->tblp->getInteger(row, col_partner_name));
    d.setFieldValue("lePartnerAddress", ui->tblp->getInteger(row, col_partner_address));
    d.setFieldValue("leGoodsID", ui->tblp->getInteger(row, col_goods_id));
    d.setFieldValue("leGoodsGroup", ui->tblp->getString(row, col_goods_group));
    d.setFieldValue("leBarcode", ui->tblp->getString(row, col_goods_barcode));
    d.setFieldValue("leGoodsName", ui->tblp->getString(row, col_goods_name));
    d.setFieldValue("leSpecialPrice", ui->tblp->getDouble(row, col_goods_special_price));
    d.setFieldValue("leRetailPrice", ui->tblp->getDouble(row, col_goods_retail));
    d.setFieldValue("leWhosalePrice", ui->tblp->getDouble(row, col_goods_special_price));
    d.setFieldValue("leRetailPriceDiff", ui->tblp->getDouble(row, col_goods_special_price));
    d.setFieldValue("leWhosalePriceDiff", ui->tblp->getDouble(row, col_goods_special_price));
    if (d.exec() == QDialog::Accepted) {
        fillRow( &d, row);
    }
}

void C5GoodsSpecialPrices::on_btnAdd_clicked()
{
    C5GoodsSpecialPrice d;
    if (d.exec() == QDialog::Accepted) {
        fillRow( &d, -1);
    }
}

void C5GoodsSpecialPrices::on_btnSave_clicked()
{
    C5Database db;
    db.exec("delete from c_goods_special_prices");
    for (int i = 0; i < ui->tblp->rowCount(); i++) {
        db[":f_partner"] = ui->tblp->getInteger(i, col_partner_id);
        db[":f_goods"] = ui->tblp->getInteger(i, col_goods_id);
        db[":f_price"] = ui->tblp->getDouble(i, col_goods_special_price);
        db.insert("c_goods_special_prices", false);
    }
    C5Message::info(tr("Saved"));
}

void C5GoodsSpecialPrices::on_btnAddGroup_clicked()
{
}

void C5GoodsSpecialPrices::on_btnDeleteGroup_clicked()
{
}

void C5GoodsSpecialPrices::on_btnEdit_clicked()
{
    int row = ui->tblp->currentRow();
    if (row < 0) {
        return;
    }
    auto item = ui->tblp->item(row, 0);
    on_tblp_itemClicked(item);
}
