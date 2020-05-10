#include "searchitems.h"
#include "ui_searchitems.h"
#include "c5database.h"
#include "c5config.h"

SearchItems::SearchItems(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchItems)
{
    ui->setupUi(this);
    showMaximized();
}

SearchItems::~SearchItems()
{
    delete ui;
}

void SearchItems::on_btnSearch_clicked()
{
    ui->tbl->setRowCount(0);
    ui->leTotalQty->setDouble(0);
    if (ui->leCode->isEmpty()) {
        return;
    }
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 200, 200, 200, 100, 100, 100);
    ui->tbl->setColumnHidden(3, __c5config.shopDenyF1());
    ui->tbl->setColumnHidden(4, __c5config.shopDenyF2());
    C5Database db(__c5config.replicaDbParams());
    db[":f_date"] = QDate::currentDate();
    db[":f_scancode"] = ui->leCode->text();
    db.exec("select ss.f_name as f_storage,g.f_name as f_goods,g.f_scancode,sum(s.f_qty*s.f_type) as f_qty, "
            "u.f_name as f_unit,g.f_saleprice,g.f_saleprice2 "
            "from a_store s "
            "inner join c_storages ss on ss.f_id=s.f_store "
            "inner join c_goods g on g.f_id=s.f_goods "
            "inner join c_units u on u.f_id=g.f_unit "
            "inner join a_header h on h.f_id=s.f_document  "
            "where  h.f_date<=:f_date and g.f_scancode=:f_scancode "
            "group by ss.f_name,g.f_name,u.f_name,g.f_saleprice,g.f_saleprice2 "
            "having sum(s.f_qty*s.f_type) > 0");
    while (db.nextRow()) {
        int r = ui->tbl->addEmptyRow();
        for (int i = 0; i < db.columnCount(); i++) {
            ui->tbl->setData(r, i, db.getValue(i));
        }
        ui->leTotalQty->setDouble(ui->leTotalQty->getDouble() + db.getDouble("f_qty"));
    }
    ui->leCode->clear();
}

void SearchItems::on_leCode_returnPressed()
{
    on_btnSearch_clicked();
}
