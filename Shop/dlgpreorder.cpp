#include "dlgpreorder.h"
#include "ui_dlgpreorder.h"
#include "dlgpreorderitem.h"
#include "printtaxn.h"
#include "worder.h"
#include "working.h"

DlgPreorder::DlgPreorder(QWidget *parent) :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgPreorder),
    fWorking(static_cast<Working*>(parent))
{
    ui->setupUi(this);
    showMaximized();
    loadRows("");
}

DlgPreorder::~DlgPreorder()
{
    delete ui;
}

void DlgPreorder::on_btnNew_clicked()
{
    if (ui->leUUID->isEmpty() == false) {
        on_btnSave_clicked();
    }
    ui->leUUID->clear();
    ui->leDateCreated->setDate(QDate::currentDate());
    ui->leDateFor->clear();
    ui->leCustomer->clear();
    ui->lePhone->clear();
    ui->lePrepaidCart->clear();
    ui->lePrepaidCash->clear();
    ui->lePrepaidFiscal->clear();
    ui->cbState->setCurrentIndex(0);
}

void DlgPreorder::on_btnSave_clicked()
{
    C5Database db(__c5config.replicaDbParams());
    bool isNew = false;
    if (ui->leUUID->isEmpty()) {
        isNew = true;
        ui->leUUID->setText(db.uuid());
        db[":f_id"] = ui->leUUID->text();
        db[":f_datecreated"] = QDate::currentDate();
        db[":f_timecreated"] = QTime::currentTime();
    }
    db[":f_state"] = ui->cbState->currentIndex() + 1;
    db[":f_datefor"] = ui->leDateFor->date();
    db[":f_customer"] = ui->leCustomer->text();
    db[":f_phone"] = ui->lePhone->text();
    db[":f_prepaidcash"] = ui->lePrepaidCash->getDouble();
    db[":f_prepaidcard"] = ui->lePrepaidCart->getDouble();
    db[":f_prepaidfiscal"] = ui->lePrepaidFiscal->getInteger();
    if (isNew) {
        db.insert("p_preorder_header", false);
    } else {
        db.update("p_preorder_header", "f_id", ui->leUUID->text());
    }
    db[":f_header"] = ui->leUUID->text();
    db.exec("delete from p_preorder_body where f_header=:f_header");
    for (int i = 0; i < ui->tgoods->rowCount(); i++) {
        db[":f_id"] = db.uuid();
        db[":f_header"] = ui->leUUID->text();
        db[":f_goods"] = ui->tgoods->item(i, 1)->data(Qt::EditRole);
        db[":f_qty"] = ui->tgoods->item(i, 4)->data(Qt::EditRole);
        db[":f_price"] = ui->tgoods->item(i, 5)->data(Qt::EditRole);
        db.insert("p_preorder_body", false);
    }
    loadRows(ui->leUUID->text());
}

void DlgPreorder::loadRows(const QString &id)
{
    QString sql = "select o.f_id, s.f_name, o.f_datecreated, o.f_datefor, o.f_customer, o.f_phone, "
                    "o.f_prepaidcash, o.f_prepaidcard, o.f_prepaidfiscal from p_preorder_header o "
                    "left join p_preorder_state s on s.f_id=o.f_state "
                    "where o.f_id is not null ";
    if (!id.isEmpty()) {
        sql += QString(" and o.f_id='%1'").arg(id);
    }
    if (ui->chShowAll->isChecked()) {

    } else {
        sql += " and o.f_state=1 ";
    }
    C5Database db(__c5config.dbParams());
    db.exec(sql);
    while (db.nextRow()) {
        int row = -1;
        if (!id.isEmpty()) {
            for (int i = 0; i < ui->tbl->rowCount(); i++) {
                if (ui->tbl->item(i, 0)->text() == id) {
                    row = i;
                    break;
                }
            }
        }
        if (row == -1) {
            ui->tbl->setRowCount(ui->tbl->rowCount() + 1);
            row = ui->tbl->rowCount() -1;
        }
        for (int i = 0; i < db.columnCount(); i++) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setData(Qt::EditRole, db.getValue(i));
            ui->tbl->setItem(row, i, item);
        }
    }
    ui->tbl->resizeColumnsToContents();
    ui->tbl->setColumnWidth(0, 0);
}

void DlgPreorder::loadPreorder(const QString &id)
{
    C5Database db(__c5config.replicaDbParams());
    db[":f_id"] = id;
    db.exec("select * from p_preorder_header where f_id=:f_id");
    db.nextRow();
    ui->leUUID->setText(db.getString("f_id"));
    ui->cbState->setCurrentIndex(db.getInt("f_state") - 1);
    ui->leDateCreated->setDate(db.getDate("f_datecreated"));
    ui->leDateFor->setDate(db.getDate("f_datefor"));
    ui->leCustomer->setText(db.getString("f_customer"));
    ui->lePhone->setText(db.getString("f_phone"));
    ui->lePrepaidCash->setDouble(db.getDouble("f_prepaidcash"));
    ui->lePrepaidCart->setDouble(db.getDouble("f_prepaidcard"));
    ui->lePrepaidFiscal->setInteger(db.getInt("f_prepaidfiscal"));

    ui->tgoods->resizeColumnsToContents();
    ui->tgoods->setColumnWidth(0, 0);
    ui->tgoods->setColumnWidth(1, 0);

    ui->tgoods->clearContents();
    ui->tgoods->setRowCount(0);
    db[":f_header"] = ui->leUUID->text();
    db.exec("select p.f_id, p.f_goods, g.f_scancode, g.f_name, p.f_qty, p.f_price, p.f_qty*p.f_price "
           "from p_preorder_body p "
            "left join c_goods g on g.f_id=p.f_goods "
            "where p.f_header=:f_header");
    while (db.nextRow()) {
        int r = ui->tgoods->rowCount();
        ui->tgoods->setRowCount(r + 1);
        for (int i = 0; i < ui->tgoods->columnCount(); i++) {
            ui->tgoods->setItem(r, i, new QTableWidgetItem(db.getString(i)));
        }
    }
    ui->tgoods->resizeColumnsToContents();
}

void DlgPreorder::on_btnClose_clicked()
{
    reject();
}
void DlgPreorder::on_btnPrintFiscal_clicked()
{
    if (ui->leUUID->isEmpty()) {
        C5Message::error(tr("Save first"));
        return;
    }
    C5Database db(__c5config.replicaDbParams());
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), "true", C5Config::taxCashier(), C5Config::taxPin(), 0);
    QString in, out, err;
    if (pt.printAdvanceJson(ui->lePrepaidCash->getDouble(), ui->lePrepaidCart->getDouble(), in, out, err) != pt_err_ok) {
        C5Message::error(err);
    } else {
        QString firm, hvhh, fiscal, number, sn, address, devnum, time;
        pt.parseResponse(out, firm, hvhh, fiscal, number, sn, address, devnum, time);
        ui->lePrepaidFiscal->setText(number);
        on_btnSave_clicked();
    }
    db[":f_id"] = db.uuid();
    db[":f_order"] = ui->leUUID->text();
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db[":f_in"] = in;
    db[":f_out"] = out;
    db[":f_err"] = err;
    db.insert("o_tax_log", false);
}

void DlgPreorder::on_btnEdit_clicked()
{
    int r = ui->tbl->currentRow();
    if (r < 0) {
        return;
    }
    ui->winfo->setVisible(true);
    loadPreorder(ui->tbl->item(r, 0)->text());
}

void DlgPreorder::on_tbl_cellClicked(int row, int column)
{
    if (row < 0) {
        return;
    }
    if (ui->leUUID->isEmpty() == false) {
        on_btnSave_clicked();
    }
    loadPreorder(ui->tbl->item(row, 0)->text());
}


void DlgPreorder::on_btnAddGoods_clicked()
{
    DlgPreorderItem d(this);
    if (d.exec() == QDialog::Accepted) {
        ui->tgoods->setRowCount(ui->tgoods->rowCount() + 1);
        d.configToTable(ui->tgoods, ui->tgoods->rowCount() - 1);
    }
}

void DlgPreorder::on_btnEditGoods_clicked()
{
    int r = ui->tgoods->currentRow();
    if (r < 0) {
        return;
    }
    DlgPreorderItem d(this);
    d.configFromTable(ui->tgoods, r);
    if (d.exec() == QDialog::Accepted) {
        d.configToTable(ui->tgoods, r);
    }
}

void DlgPreorder::on_btnCreateSale_clicked()
{
    if (ui->leUUID->isEmpty()) {
        C5Message::error(tr("Save first"));
        return;
    }
    for (int i = 0; i < ui->tgoods->rowCount(); i++) {
        fWorking->worder()->addGoodsToTable(ui->tgoods->item(i, 1)->data(Qt::EditRole).toInt());
        fWorking->worder()->changeQty(str_float(ui->tgoods->item(i, 4)->text()));
        fWorking->worder()->changePrice(str_float(ui->tgoods->item(i, 5)->text()));
    }
    fWorking->worder()->setPrepaid(ui->lePrepaidCart->getDouble() + ui->lePrepaidCash->getDouble());
    accept();
}
