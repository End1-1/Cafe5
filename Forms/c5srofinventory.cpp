#include "c5srofinventory.h"
#include "ui_c5srofinventory.h"
#include "c5database.h"
#include "c5message.h"

C5SrOfInventory::C5SrOfInventory(QWidget *parent) :
    C5Dialog(),
    ui(new Ui::C5SrOfInventory)
{
    ui->setupUi(this);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 0, 0, 200, 80, 0, 200, 80);
}

C5SrOfInventory::~C5SrOfInventory()
{
    delete ui;
}

void C5SrOfInventory::setGoods(const QDate &date, int store, int goods)
{
    C5Database db;
    db[":f_date"] = date;
    db[":f_store"] = store;
    db[":f_goods"] = goods;
    db.exec("select sr.f_id, sr.f_rec, sr.f_dish, g1.f_name as sr_name, sr.f_dish_qty, "
            "sr.f_goods, g2.f_name as goods_name, sr.f_goods_qty "
            "from a_header_sr sr "
            "left join c_goods g1 on g1.f_id=sr.f_dish "
            "left join c_goods g2 on g2.f_id=sr.f_goods "
            "where sr.f_date=:f_date and sr.f_store=:f_store "
            "and sr.f_dish in (select f_dish from a_header_sr store where f_date=:f_date and f_store=:f_store and f_goods=:f_goods) ");

    while(db.nextRow()) {
        int r = ui->tbl->rowCount();
        ui->tbl->setRowCount(r + 1);

        for(int i = 0; i < ui->tbl->columnCount(); i++) {
            ui->tbl->setData(r, i, db.getValue(i));
        }
    }

    for(int i = 1; i < ui->tbl->rowCount(); i++) {
        if(ui->tbl->getString(i, 1) != ui->tbl->getString(i - 1, 1)) {
            ui->tbl->insertRow(i);
            i++;
        }
    }
}

void C5SrOfInventory::on_btnDelete_clicked()
{
    if(ui->tbl->currentRow() < 0) {
        return;
    }

    fUuid = ui->tbl->getString(ui->tbl->currentRow(), 1);

    if(fUuid.isEmpty()) {
        return;
    }

    if(C5Message::question(tr("Confirm to remove")) != QDialog::Accepted) {
        return;
    }

    accept();
}
