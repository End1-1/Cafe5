#include "c5tempsale.h"
#include "ui_c5tempsale.h"
#include "working.h"
#include "c5config.h"
#include "c5database.h"
#include "c5message.h"
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

QString C5TempSale::openDraft()
{
    int r = ui->tbl->currentRow();
    if (r < 0) {
        return "";
    }
    QString draftId = ui->tbl->item(r, 0)->text();
    return draftId;
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
        C5Database db(__c5config.dbParams());
        db[":f_id"] = ui->tbl->item(r, 0)->text();
        db.exec("update o_draft_sale set f_state=4 where f_id=:f_id");
        db[":f_header"] = ui->tbl->item(r, 0)->text();
        db.exec("update o_draft_sale_body set f_state=4 where f_header=:f_header");
        refreshData();
    }
}

void C5TempSale::refreshData()
{
    C5Database db(__c5config.dbParams());
    db[":f_hall"] = __c5config.defaultHall();
    db.exec("select d.f_id, d.f_date, d.f_time, concat_ws(' ', u.f_last, u.f_first), "
            "d.f_amount "
            "from o_draft_sale d "
            "left join s_user u on u.f_id=d.f_staff "
            "where d.f_state=1 ");
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
