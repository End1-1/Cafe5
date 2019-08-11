#include "dlgchangeoutputstore.h"
#include "ui_dlgchangeoutputstore.h"
#include "c5lineedit.h"
#include "c5checkbox.h"
#include "c5cache.h"

DlgChangeOutputStore::DlgChangeOutputStore(const QStringList &dbParams, QWidget *parent) :
    C5Dialog(dbParams),
    ui(new Ui::DlgChangeOutputStore)
{
    ui->setupUi(this);
    ui->tbl->setRowCount(1);
    for (int i = 0; i < ui->tbl->columnCount(); i++){
        C5LineEdit *l = ui->tbl->createLineEdit(0, i);
        connect(l, SIGNAL(textChanged(QString)), this, SLOT(search(QString)));
    }
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 0, 100, 100, 200, 250, 100, 100, 30);
    ui->leStore->setSelector(fDBParams, ui->leStoreName, cache_goods_store);
}

DlgChangeOutputStore::~DlgChangeOutputStore()
{
    delete ui;
}

void DlgChangeOutputStore::refresh(const QDate &d1, const QDate &d2)
{
    ui->deStart->setDate(d1);
    ui->deEnd->setDate(d2);
    for (int i = 0; i < ui->tbl->columnCount(); i++) {
        ui->tbl->lineEdit(0, i)->clear();
    }
    C5Database db(fDBParams);
    db[":f_datecash1"] = d1;
    db[":f_datecash2"] = d2;
    db[":f_headerstate"] = ORDER_STATE_CLOSE;
    db[":f_bodystate1"] = DISH_STATE_OK;
    db[":f_bodystate2"] = DISH_STATE_VOID;
    db.exec("select h.f_id as f_header, b.f_id as f_body, hl.f_name as f_hallname, st.f_name as f_store, "
            "p2.f_name as f_part2name, d.f_name as f_dishname, b.f_qty1, b.f_total "
            "from o_body b "
            "inner join o_header h on h.f_id=b.f_header "
            "left join c_storages st on st.f_id=b.f_store "
            "left join d_dish d on d.f_id=b.f_dish "
            "left join d_part2 p2 on p2.f_id=d.f_part "
            "left join h_halls hl on hl.f_id=h.f_hall "
            "where h.f_datecash between :f_datecash1 and :f_datecash2 and h.f_state=:f_headerstate "
            "and (b.f_state=:f_bodystate1 or b.f_state=:f_bodystate2)");
    ui->tbl->setRowCount(1 + db.rowCount());
    int row = 1;
    while (db.nextRow()) {
        for (int i = 0; i < ui->tbl->columnCount() - 1; i++) {
            ui->tbl->setData(row, i, db.getValue(i));
        }
        ui->tbl->createCheckbox(row, 8);
        row++;
    }
}

void DlgChangeOutputStore::searchInTable(int col, const QString &str)
{
    for (int i = 1; i < ui->tbl->rowCount(); i++){
        bool rh = true;
        rh = ui->tbl->getString(i, col).contains(str, Qt::CaseInsensitive);
        ui->tbl->setRowHidden(i, !rh);
    }
}

void DlgChangeOutputStore::search(const QString &arg1)
{
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());
    int r, c;
    if (!ui->tbl->findWidget(l, r, c)) {
        return;
    }
    searchInTable(c, arg1);
}

void DlgChangeOutputStore::on_btnCheckVisible_clicked()
{
    for (int i = 1; i < ui->tbl->rowCount(); i++) {
        ui->tbl->checkBox(i, 8)->setChecked(!ui->tbl->isRowHidden(i));
    }
}

void DlgChangeOutputStore::on_btnRefresh_clicked()
{
    refresh(ui->deStart->date(), ui->deEnd->date());
}

void DlgChangeOutputStore::on_btnGo_clicked()
{
    if (ui->leStore->getInteger() == 0) {
        C5Message::error(tr("Store is not selected"));
        return;
    }
    if (C5Message::question(tr("Are you sure to change store?")) != QDialog::Accepted) {
        return;
    }
    C5Database db(fDBParams);
    for (int i = 1; i < ui->tbl->rowCount(); i++) {
        if (ui->tbl->checkBox(i, 8)->isChecked()) {
            db[":f_store"] = ui->leStore->getInteger();
            db[":f_id"] = ui->tbl->getString(i, 1);
            db.exec("update o_body set f_store=:f_store where f_id=:f_id");
        }
    }
    C5Message::info(tr("Done"));
}
