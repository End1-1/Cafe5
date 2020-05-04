#include "storeinput.h"
#include "ui_storeinput.h"
#include "c5utils.h"
#include "c5database.h"
#include "c5config.h"
#include "c5storedraftwriter.h"
#include "c5checkbox.h"
#include "c5message.h"
#include "viewinputitem.h"

StoreInput::StoreInput(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StoreInput)
{
    ui->setupUi(this);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 30, 100, 0, 300, 140, 100, 100);
    getList();
}

StoreInput::~StoreInput()
{
    delete ui;
}

void StoreInput::on_btnView_clicked()
{
    QModelIndexList l = ui->tbl->selectionModel()->selectedRows();
    if (l.count() == 0) {
        return;
    }
    ViewInputItem *vi = new ViewInputItem(ui->tbl->getString(l.at(0).row(), 0));
    vi->exec();
    vi->deleteLater();
}

void StoreInput::on_btnAccept_clicked()
{
    QList<int> rows;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        if (ui->tbl->checkBox(i, 1)->isChecked()) {
            rows.append(i);
        }
    }
    if (rows.isEmpty()) {
        C5Message::error(tr("Nothing is selected"));
        return;
    }
    C5Database db(__c5config.replicaDbParams());
    C5StoreDraftWriter dw(db);
    foreach (int r, rows) {
        db[":f_id"] = ui->tbl->getString(r, 0);
        db.insert("a_header_shop2partneraccept", false);
    }
    getList();
}

void StoreInput::getList()
{
    ui->tbl->clearContents();
    ui->tbl->setRowCount(0);
    C5Database db(__c5config.replicaDbParams());
    db[":f_store"] = __c5config.defaultStore();
    db.exec("select ad.f_id, spa.f_id as f_acc, ah.f_date, g.f_name, g.f_scancode, ad.f_qty, g.f_saleprice "
            "from a_store_draft ad "
            "left join a_header_shop2partneraccept spa on spa.f_id=ad.f_id "
            "inner join c_goods g on g.f_id=ad.f_goods "
            "inner join a_header ah on ah.f_id=ad.f_document "
            "inner join a_header_shop2partner sp on sp.f_id=ah.f_id "
            "where sp.f_store=:f_store and sp.f_accept=0 ");
    while (db.nextRow()) {
        if (!db.getString("f_acc").isEmpty()) {
            continue;
        }
        int r = ui->tbl->addEmptyRow();
        ui->tbl->setString(r, 0, db.getString("f_id"));
        ui->tbl->createCheckbox(r, 1);
        ui->tbl->setString(r, 2, db.getDate("f_date").toString(FORMAT_DATE_TO_STR));
        ui->tbl->setString(r, 3, "Store");
        ui->tbl->setString(r, 4, db.getString("f_name"));
        ui->tbl->setString(r, 5, db.getString("f_scancode"));
        ui->tbl->setString(r, 6, db.getString("f_qty"));
        ui->tbl->setString(r, 7, db.getString("f_saleprice"));
    }
}
