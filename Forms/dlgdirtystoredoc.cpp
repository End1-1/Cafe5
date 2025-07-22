#include "dlgdirtystoredoc.h"
#include "ui_dlgdirtystoredoc.h"
#include "c5lineedit.h"
#include "c5message.h"
#include "c5database.h"

DlgDirtyStoreDoc::DlgDirtyStoreDoc(const QString &uuid) :
    C5Dialog(),
    ui(new Ui::DlgDirtyStoreDoc),
    fUuid(uuid)
{
    ui->setupUi(this);
    C5Database db;
    db[":f_document"] = uuid;
    db.exec("select s.f_id, s.f_goods, g.f_scancode, g.f_name, s.f_qty, 0, s.f_price, s.f_total "
            "from a_store s "
            "left join c_goods g on g.f_id=s.f_goods "
            "where s.f_document=:f_document ");
    ui->tbl->setRowCount(db.rowCount());
    int r = 0;
    QMap<QString, int> rowsId;
    while (db.nextRow()) {
        for (int i = 0; i < db.columnCount(); i++) {
            if (i == 0) {
                rowsId[db.getString(0)] = r;
            }
            ui->tbl->setData(r, i, db.getValue(i));
        }
        r++;
    }
    db[":f_document"] = uuid;
    db.exec("select s.f_base, sum(s.f_qty)*-1 "
            "from a_store s "
            "left join a_store ss on ss.f_id=s.f_base "
            "where ss.f_document=:f_document and s.f_type=-1 "
            "group by 1 ");
    while (db.nextRow()) {
        ui->tbl->setDouble(rowsId[db.getString(0)], 5, db.getDouble(1));
    }
    countTotal();
    std::vector<int> colswidths;
    colswidths.push_back(0);
    colswidths.push_back(0);
    colswidths.push_back(100);
    colswidths.push_back(300);
    colswidths.push_back(80);
    colswidths.push_back(80);
    colswidths.push_back(80);
    colswidths.push_back(80);
    for (int i = 0; i < colswidths.size(); i++) {
        ui->tbl->setColumnWidth(i, colswidths.at(i));
    }
}

DlgDirtyStoreDoc::~DlgDirtyStoreDoc()
{
    delete ui;
}

void DlgDirtyStoreDoc::on_btnEdit_clicked()
{
    int r = ui->tbl->currentRow();
    if (r < 0) {
        return;
    }
    C5LineEdit *l = ui->tbl->createLineEdit(r, 4);
    l->setDouble(ui->tbl->getDouble(r, 4));
    connect(l, &C5LineEdit::textChanged, this, [this]() {
        int row, col;
        if (ui->tbl->findWidget(static_cast<QWidget * >(sender()), row, col)) {
            ui->tbl->lineEdit(row, 7)->setDouble(ui->tbl->getDouble(row, 4) *ui->tbl->getDouble(row, 6));
            countTotal();
        }
    });
    l = ui->tbl->createLineEdit(r, 6);
    l->fDecimalPlaces = 4;
    l->setValidator(new QDoubleValidator(0, 999999999, 4));
    l->setDouble(ui->tbl->getDouble(r, 6));
    connect(l, &C5LineEdit::textChanged, this, [this]() {
        int row, col;
        if (ui->tbl->findWidget(static_cast<QWidget * >(sender()), row, col)) {
            ui->tbl->lineEdit(row, 7)->setDouble(ui->tbl->getDouble(row, 6) *ui->tbl->getDouble(row, 4));
            countTotal();
        }
    });
    l = ui->tbl->createLineEdit(r, 7);
    l->setDouble(ui->tbl->getDouble(r, 7));
    connect(l, &C5LineEdit::textChanged, this, [this, &r]() {
        int row, col;
        if (ui->tbl->findWidget(static_cast<QWidget * >(sender()), row, col)) {
            if (ui->tbl->getDouble(row, 4) > 0.001) {
                ui->tbl->lineEdit(row, 6)->setDouble(ui->tbl->lineEdit(row, 7)->getDouble() / ui->tbl->lineEdit(row, 4)->getDouble());
                countTotal();
            }
        }
    });
}

void DlgDirtyStoreDoc::on_tbl_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    Q_UNUSED(previousColumn);
    Q_UNUSED(currentColumn);
    if (previousRow > -1 && previousRow != currentRow) {
        ui->tbl->removeCellWidget(previousRow, 4);
        ui->tbl->removeCellWidget(previousRow, 6);
        ui->tbl->removeCellWidget(previousRow, 7);
    }
}

void DlgDirtyStoreDoc::on_btnRemove_clicked()
{
    int r = ui->tbl->currentRow();
    if (r < 0) {
        return;
    }
    if (ui->tbl->getDouble(r, 5) < 0.001) {
        C5Message::error(tr("Cannot remove goods that was used"));
        return;
    }
    if (C5Message::question(tr("Confirm remove")) != QDialog::Accepted) {
        return;
    }
    C5Database db;
    db[":f_id"] = ui->tbl->getString(r, 0);
    db.exec("delete from a_store where f_id=:f_id");
    db[":f_id"] = ui->tbl->getString(r, 0);
    db.exec("delete from a_store_draft where f_id=:f_id");
    ui->tbl->removeRow(r);
    countTotal();
}

void DlgDirtyStoreDoc::countTotal()
{
    double total = 0;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        total += ui->tbl->getDouble(i, 7);
    }
    ui->leTotal->setDouble(total);
}

void DlgDirtyStoreDoc::correctDocumentAmount()
{
    C5Database db;
    db[":f_document"] = fUuid;
    db.exec("select sum(f_total) from a_store where f_document=:f_document");
    db.nextRow();
    double total = db.getDouble(0);
    db[":f_amount"] = total;
    db.update("a_header", "f_id", fUuid);
    db[":f_storedoc"] = fUuid;
    db.exec("select f_id from a_header_cash where f_storedoc=:f_storedoc");
    if (db.nextRow()) {
        db[":f_amount"] = total;
        db.update("e_cash", "f_header", db.getString(0));
    }
}

void DlgDirtyStoreDoc::on_btnSave_clicked()
{
    int r = ui->tbl->currentRow();
    if (r < 0) {
        return;
    }
    if (ui->tbl->lineEdit(r, 4) == nullptr) {
        return;
    }
    if (ui->tbl->getDouble(r, 5) > ui->tbl->lineEdit(r, 4)->getDouble()) {
        C5Message::error(tr("New quantity less than used"));
        return;
    }
    C5Database db;
    db[":f_qty"] = ui->tbl->lineEdit(r, 4)->getDouble();
    db[":f_price"] = ui->tbl->lineEdit(r, 6)->getDouble();
    db[":f_total"] = ui->tbl->lineEdit(r, 7)->getDouble();
    db.update("a_store", "f_id", ui->tbl->getString(r, 0));
    db[":f_qty"] = ui->tbl->lineEdit(r, 4)->getDouble();
    db[":f_price"] = ui->tbl->lineEdit(r, 6)->getDouble();
    db[":f_total"] = ui->tbl->lineEdit(r, 7)->getDouble();
    db.update("a_store_draft", "f_id", ui->tbl->getString(r, 0));
    db[":f_base"] = ui->tbl->getString(r, 0);
    db[":f_price"] = ui->tbl->lineEdit(r, 6)->getDouble();
    db.exec("update a_store_draft set f_price=:f_price, f_total=f_qty*:f_price "
            "where f_id in(select f_draft from a_store where f_base=:f_base) and f_type=-1");
    db[":f_base"] = ui->tbl->getString(r, 0);
    db[":f_price"] = ui->tbl->lineEdit(r, 6)->getDouble();
    db.exec("update a_store set f_price=:f_price, f_total=f_qty*:f_price where f_base=:f_base and f_type=-1");
    ui->tbl->removeCellWidget(r, 4);
    ui->tbl->removeCellWidget(r, 6);
    ui->tbl->removeCellWidget(r, 7);
    correctDocumentAmount();
}
