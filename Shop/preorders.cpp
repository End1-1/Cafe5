#include "preorders.h"
#include "c5database.h"
#include "c5config.h"
#include "ui_preorders.h"

Preorders::Preorders() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::Preorders)
{
    ui->setupUi(this);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 100, 80, 100, 80, 250, 150, 100, 100, 100);
    C5Database db(__c5config.replicaDbParams());
    db[":f_state"] = 1;
    db.exec("select o.f_id,  o.f_datecreate, o.f_timecreate, o.f_datefor, o.f_timefor, "
            "c.f_contact, c.f_phone, o.f_total, o.f_advance, o.f_advancetax "
            "from op_header o "
            "inner join op_header_state os on os.f_id=o.f_state "
            "left join c_partners c on c.f_id=o.f_customer "
            "where o.f_state=:f_state ");
    while (db.nextRow()) {
        int r = ui->tbl->addEmptyRow();
        ui->tbl->setData(r, 0, db.getValue("f_id"));
        ui->tbl->setData(r, 1, db.getDate("f_datecreate"));
        ui->tbl->setData(r, 2, db.getTime("f_timecreate"));
        ui->tbl->setData(r, 3, db.getDate("f_datefor"));
        ui->tbl->setData(r, 4, db.getTime("f_timefor"));
        ui->tbl->setData(r, 5, db.getString("f_contact"));
        ui->tbl->setData(r, 6, db.getString("f_phone"));
        ui->tbl->setData(r, 7, db.getDouble("f_total"));
        ui->tbl->setData(r, 8, db.getDouble("f_advance"));
        ui->tbl->setData(r, 9, db.getString("f_advancetax"));
        ui->leTotalAmount->setDouble(ui->leTotalAmount->getDouble() + db.getDouble("f_total"));
        ui->leTotalAdvance->setDouble(ui->leTotalAdvance->getDouble() + db.getDouble("f_advance"));
    }
    showMaximized();
}

Preorders::~Preorders()
{
    delete ui;
}

void Preorders::on_btnView_clicked()
{
    QModelIndexList ml = ui->tbl->selectionModel()->selectedRows();
    if (ml.count() == 0) {
        return;
    }
    fUUID = ui->tbl->item(ml.at(0).row(), 0)->data(Qt::EditRole).toByteArray();
    accept();
}

void Preorders::on_btnExit_clicked()
{
    reject();
}
void Preorders::on_btnRemove_clicked()
{
    QModelIndexList ml = ui->tbl->selectionModel()->selectedRows();
    if (ml.count() == 0) {
        return;
    }
    fUUID = ui->tbl->item(ml.at(0).row(), 0)->data(Qt::EditRole).toByteArray();
    if (C5Message::question(tr("Confirm to remove preorder")) != QDialog::Accepted) {
        return;
    }
    C5Database db(__c5config.dbParams());
    db[":f_id"] = fUUID;
    db.exec("update op_header set f_state=3 where f_id=:f_id");
    ui->tbl->removeRow(ml.at(0).row());
}
