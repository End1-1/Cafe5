#include "c5salefromstoreorder.h"
#include "ui_c5salefromstoreorder.h"
#include "c5mainwindow.h"
#include "c5storedraftwriter.h"
#include "printtaxn.h"
#include "c5cache.h"
#include <QMenu>

C5SaleFromStoreOrder::C5SaleFromStoreOrder(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::C5SaleFromStoreOrder)
{
    ui->setupUi(this);
    ui->tblData->setColumnWidths(ui->tblData->columnCount(), 0, 0, 300, 80, 80, 80, 80, 0);
    ui->leID->setVisible(false);
    ui->lePartner->setSelector(dbParams, ui->lePartnerName, cache_goods_partners);
    ui->leSeller->setSelector(dbParams, ui->leSellerName, cache_users);
    //ui->btnRemove->setVisible(pr(fDBParams, cp_t5_refund_goods));
}

C5SaleFromStoreOrder::~C5SaleFromStoreOrder()
{
    delete ui;
}

void C5SaleFromStoreOrder::openOrder(const QStringList &dbParams, const QString &id)
{
    C5SaleFromStoreOrder *d = new C5SaleFromStoreOrder(dbParams);
    d->loadOrder(id);
    d->exec();
    delete d;
}

void C5SaleFromStoreOrder::loadOrder(const QString &id)
{
    ui->leID->setText(id);
    C5Database db(fDBParams);
    db[":f_id"] = id;
    db.exec("select * from o_header where f_id=:f_id");
    if (db.nextRow()) {
        ui->deDate->setDate(db.getDate("f_datecash"));
        ui->teTime->setText(db.getString("f_timeclose"));
        ui->leUserId->setText(db.getString("f_prefix") + db.getString("f_hallid"));
        ui->leTotalCash->setDouble(db.getDouble("f_amountcash"));
        ui->leTotalCard->setDouble(db.getDouble("f_amountcard"));
        ui->lePartner->setValue(db.getString("f_partner"));
        ui->btnRemove->setVisible(db.getInt("f_source") > 0);
        ui->leSeller->setValue(db.getInt("f_staff"));
    } else {
        C5Message::error(tr("No such order"));
        return;
    }
    db[":f_order"] = ui->leID->text();
    db.exec("select * from b_clients_debts where f_order=:f_order");
    ui->chDebt->setChecked(db.nextRow());
    db[":f_id"] = id;
    db.exec("select og.f_id, og.f_goods, g.f_name, og.f_qty, gu.f_name, og.f_price, og.f_total,"
            "t.f_taxdept, t.f_adgcode, "
            "og.f_store "
            "from o_goods og "
            "left join c_goods g on g.f_id=og.f_goods "
            "left join c_units gu on gu.f_id=g.f_unit "
            "left join c_groups t on t.f_id=g.f_group "
            "where og.f_header=:f_id");
    while (db.nextRow()) {
        int row = ui->tblData->addEmptyRow();
        ui->tblData->setString(row, 0, db.getString(0));
        ui->tblData->setInteger(row, 1, db.getInt(1));
        ui->tblData->setString(row, 2, db.getString(2));
        ui->tblData->setDouble(row, 3, db.getDouble(3));
        ui->tblData->setString(row, 4, db.getString(4));
        ui->tblData->setDouble(row, 5, db.getDouble(5));
        ui->tblData->setDouble(row, 6, db.getDouble(6));
        ui->tblData->setInteger(row, 7, db.getInt(7));
        ui->tblData->setString(row, 8, db.getString("f_taxdept"));
        ui->tblData->setString(row, 9, db.getString("f_adgcode"));
    }
    db[":f_id"] = id;
    db.exec("select * from o_tax where f_id=:f_id");
    ui->btnPrintTax->setVisible(!db.nextRow());
}

void C5SaleFromStoreOrder::on_btnRemove_clicked()
{
    if (C5Message::question(tr("Confirm to remove")) != QDialog::Accepted) {
        return;
    }
    C5Database db(fDBParams);
    db.startTransaction();
    QStringList storeRec;
    db[":f_header"] = ui->leID->text();
    db.exec("select f_storerec from o_goods where f_header=:f_header");
    while (db.nextRow()) {
        storeRec.append(db.getString(0));
    }
    QSet<QString> storeHeader;
    foreach (const QString &s, storeRec) {
        db[":f_id"] = s;
        db.exec("select f_document from a_store_draft where f_id=:f_id");
        if (db.nextRow()) {
            storeHeader.insert(db.getString(0));
        }
    }
    foreach (const QString &s, storeHeader) {
        db[":f_document"] = s;
        db.exec("delete from a_store where f_document=:f_document");
        db[":f_document"] = s;
        db.exec("delete from a_store_draft where f_document=:f_document");
        db[":f_id"] = s;
        db.exec("delete from a_header_store where f_id=:f_id");
        db[":f_id"] = s;
        db.exec("delete from a_header where f_id=:f_id");
    }

    storeHeader.clear();
    db[":f_oheader"] = ui->leID->text();
    db.exec("select f_id from a_header_cash where f_oheader=:f_oheader");
    if (db.nextRow()) {
        storeHeader.insert(db.getString(0));
    }
    foreach (const QString &s, storeHeader) {
        db[":f_header"] = s;
        db.exec("delete from e_cash where f_header=:f_header");
        db[":f_id"] = s;
        db.exec("delete from a_header_cash where f_id=:f_id");
        db[":f_id"] = s;
        db.exec("delete from a_header where f_id=:f_id");
    }

    db[":f_header"] = ui->leID->text();
    db.exec("delete from o_goods where f_header=:f_header");
    db[":f_id"] = ui->leID->text();
    db.exec("delete from o_header where f_id=:f_id");

    db.deleteFromTable("b_clients_debts", "f_order", ui->leID->text());
    db.commit();
    ui->btnRemove->setVisible(false);
    C5Message::info(tr("Removal complete"));
}

void C5SaleFromStoreOrder::on_btnPrintTax_clicked()
{
    C5Database db(fDBParams);
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(), C5Config::taxCashier(), C5Config::taxPin(), this);
    for (int i = 0; i < ui->tblData->rowCount(); i++) {
        pt.addGoods(ui->tblData->getString(i, 8), //dep
                    ui->tblData->getString(i, 9), //adg
                    ui->tblData->getString(i, 1), //goods id
                    ui->tblData->getString(i, 2), //name
                    ui->tblData->getDouble(i, 5), //price
                    ui->tblData->getDouble(i, 3), //qty
                    0); //discount
    }
    QString jsonIn, jsonOut, err;
    QString sn, firm, address, fiscal, hvhh, rseq, devnum, time;
    int result = 0;
    result = pt.makeJsonAndPrint(ui->leTotalCard->getDouble(), 0, jsonIn, jsonOut, err);

    db[":f_id"] = db.uuid();
    db[":f_order"] = ui->leID->text();
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db[":f_in"] = jsonIn;
    db[":f_out"] = jsonOut;
    db[":f_err"] = err;
    db[":f_result"] = result;
    db.insert("o_tax_log", false);
    QSqlQuery *q = new QSqlQuery(db.fDb);
    if (result == pt_err_ok) {
        PrintTaxN::parseResponse(jsonOut, firm, hvhh, fiscal, rseq, sn, address, devnum, time);
        db[":f_id"] = ui->leID->text();
        db.exec("delete from o_tax where f_id=:f_id");
        db[":f_id"] = ui->leID->text();
        db[":f_dept"] = C5Config::taxDept();
        db[":f_firmname"] = firm;
        db[":f_address"] = address;
        db[":f_devnum"] = devnum;
        db[":f_serial"] = sn;
        db[":f_fiscal"] = fiscal;
        db[":f_receiptnumber"] = rseq;
        db[":f_hvhh"] = hvhh;
        db[":f_fiscalmode"] = tr("(F)");
        db[":f_time"] = time;
        db.insert("o_tax", false);
        pt.saveTimeResult(ui->leID->text(), *q);
        delete q;
        ui->btnPrintTax->setVisible(false);
        C5Message::info(tr("Printed"));
    } else {
        pt.saveTimeResult("Not saved - " + ui->leID->text(), *q);
        delete q;
        C5Message::error(err + "<br>" + jsonOut + "<br>" + jsonIn);
    }
}

void C5SaleFromStoreOrder::on_btnSave_clicked()
{
    C5Database db(fDBParams);
    if (ui->chDebt->isChecked()) {
        if (ui->lePartner->getInteger() == 0) {
            C5Message::error(tr("Dept option is selected, but no partner selected"));
            return;
        }
        db[":f_order"] = ui->leID->text();
        db.exec("select * from b_clients_debt where f_order=:f_header");
        if (db.nextRow()) {
            db[":f_costumer"] = ui->lePartner->getInteger();
            db.update("b_clients_debts", "f_header", ui->leID->text());
        } else {
            db[":f_order"] = ui->leID->text();
            db[":f_costumer"] = ui->lePartner->getInteger();
            db[":f_date"] = ui->deDate->date();
            db[":f_amount"] = ui->leTotalCash->getDouble() * -1;
            db.insert("b_clients_debts");
        }
    } else {
        db.deleteFromTable("b_clients_debts", "f_order", ui->leID->text());
    }
    db[":f_partner"] = ui->lePartner->getInteger();
    db[":f_staff"] = ui->leSeller->getInteger();
    db.update("o_header", where_id(ui->leID->text()));

    C5Message::info(tr("Saved"));
}
