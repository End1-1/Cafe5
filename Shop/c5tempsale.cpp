#include "c5tempsale.h"
#include "ui_c5tempsale.h"
#include "worder.h"
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

void C5TempSale::openDraft(WOrder *o)
{
    int r = ui->tbl->currentRow();
    if (r < 0) {
        return;
    }
    o->fDraftSale = ui->tbl->item(r, 0)->text();
    C5Database db(__c5config.replicaDbParams());
    db[":f_header"] = o->fDraftSale;
    db.exec("select * from o_draft_sale_body where f_header=:f_header and f_state=1");
    while (db.nextRow()) {
        o->addGoods(db.getInt("f_goods"));
        o->changeQty(db.getDouble("f_qty"));
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
