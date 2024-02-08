#include "c5tempsale.h"
#include "ui_c5tempsale.h"
#include "worder.h"
#include "working.h"
#include "odraftsale.h"
#include "odraftsalebody.h"
#include <QTableWidgetItem>

C5TempSale::C5TempSale() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::C5TempSale)
{
    ui->setupUi(this);
    refreshData();
}

C5TempSale::~C5TempSale()
{
    delete ui;
}

void C5TempSale::openDraft()
{
    int r = ui->tbl->currentRow();
    if (r < 0) {
        return;
    }
    QString draftId = ui->tbl->item(r, 0)->text();
    if (Working::working()->findDraft(draftId)) {
        return;
    }
    C5Database db(__c5config.replicaDbParams());
    db[":f_id"] = draftId;
    db.exec("select * from o_draft_sale where f_id=:f_id");
    ODraftSale ds;
    ds.getRecord(db);
    WOrder *wo = Working::working()->newSale(ds.saleType);
    wo->fDraftSale.id = ds.id;
    db[":f_header"] = wo->fDraftSale.id;
    db.exec("select b.f_goods, b.f_id, b.f_qty, cp.f_price1, cp.f_price2 "
            "from o_draft_sale_body b "
            "left join c_goods_prices cp on cp.f_goods=b.f_goods and cp.f_currency=1 "
            "where f_header=:f_header and f_state=1 ");
    while (db.nextRow()) {
        wo->addGoodsToTable(db.getInt("f_goods"), true, 0, db.getString("f_id"), db.getDouble("f_price1"), db.getDouble("f_price2"));
        wo->changeQty(db.getDouble("f_qty"));
    }
}

void C5TempSale::on_tbl_itemDoubleClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }
    if (item->row() < 0) {
        return;
    }
}

void C5TempSale::on_btnExit_clicked()
{
    reject();
}

void C5TempSale::on_btnOpen_clicked()
{
    int r = ui->tbl->currentRow();
    if (r < 0) {
        return;
    }
    accept();
}

void C5TempSale::on_btnTrash_clicked()
{
    int r = ui->tbl->currentRow();
    if (r < 0) {
        return;
    }
    if (C5Message::question(tr("Confirm to remove")) == QDialog::Accepted) {
        C5Database db(__c5config.replicaDbParams());
        db[":f_id"] = ui->tbl->item(r, 0)->text();
        db.exec("update o_draft_sale set f_state=4 where f_id=:f_id");
        refreshData();
    }
}

void C5TempSale::refreshData()
{
    C5Database db(__c5config.replicaDbParams());
    db.exec("select d.f_id, d.f_date, d.f_time, concat_ws(' ', u.f_last, u.f_first), "
            "d.f_amount "
            "from o_draft_sale d "
            "left join s_user u on u.f_id=d.f_staff "
            "where d.f_state=1");
    ui->tbl->setRowCount(0);
    while (db.nextRow()) {
        int r = ui->tbl->rowCount();
        ui->tbl->setRowCount(r + 1);
        for (int i = 0; i < db.columnCount(); i++) {
            ui->tbl->setItem(r, i, new QTableWidgetItem(db.getString(i)));
        }
    }
    ui->tbl->resizeColumnsToContents();
    ui->tbl->setColumnWidth(0, 0);
}
