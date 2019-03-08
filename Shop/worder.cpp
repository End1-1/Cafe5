#include "worder.h"
#include "ui_worder.h"
#include "c5database.h"
#include "c5config.h"
#include "printtaxn.h"
#include "dqty.h"

WOrder::WOrder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WOrder)
{
    ui->setupUi(this);
    fDateOpen = QDate::currentDate();
    fTimeOpen = QTime::currentTime();
    ui->tblGoods->setColumnWidths(ui->tblGoods->columnCount(), 0, 300, 80, 80, 80, 80, 80, 80);
}

WOrder::~WOrder()
{
    delete ui;
}

void WOrder::focusCash()
{
    ui->leCash->setFocus();
}

void WOrder::focusCard()
{
    ui->leCard->setFocus();
}

void WOrder::addGoods(const Goods &g)
{
    int row = ui->tblGoods->addEmptyRow();
    ui->tblGoods->setString(row, 0, g.fCode);
    ui->tblGoods->setString(row, 1, g.fName);
    ui->tblGoods->setDouble(row, 2, 1);
    ui->tblGoods->setString(row, 3, g.fUnit);
    ui->tblGoods->setDouble(row, 4, g.fPrice);
    ui->tblGoods->setDouble(row, 5, g.fPrice);
    ui->tblGoods->setInteger(row, 6, g.fTaxDept);
    ui->tblGoods->setString(row, 7, g.fAdgCode);
    ui->tblGoods->setCurrentItem(ui->tblGoods->item(row, 0));
    countTotal();
}

bool WOrder::writeOrder(bool tax)
{
    if (ui->tblGoods->rowCount() == 0) {
        return false;
    }
    if (C5Message::question(tr("Write order?")) != QDialog::Accepted) {
        return false;
    }
    C5Database db(C5Config::dbParams());
    db.startTransaction();

    if (tax) {
        PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(), this);
        for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
            pt.addGoods(ui->tblGoods->getString(i, 6), //dep
                        ui->tblGoods->getString(i, 7), //adg
                        ui->tblGoods->getString(i, 0), //goods id
                        ui->tblGoods->getString(i, 1), //name
                        ui->tblGoods->getDouble(i, 4), //price
                        ui->tblGoods->getDouble(i, 2)); //qty
        }
        QString jsonIn, jsonOut, err;
        int result = 0;
        result = pt.makeJsonAndPrint(ui->leCard->getDouble(), 0, jsonIn, jsonOut, err);
        db[":f_order"] = 0;
        db[":f_date"] = QDate::currentDate();
        db[":f_time"] = QTime::currentTime();
        db[":f_in"] = jsonIn;
        db[":f_out"] = jsonOut;
        db[":f_err"] = err;
        db[":f_result"] = result;
        db.insert("o_tax_log", false);
        if (result == pt_err_ok) {
            QString sn, firm, address, fiscal, hvhh, rseq, devnum, time;
            PrintTaxN::parseResponse(jsonOut, firm, hvhh, fiscal, rseq, sn, address, devnum, time);
            db[":f_id"] = 0;
            db.exec("delete from o_tax where f_id=:f_id");
            db[":f_id"] = 0;
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
        } else {
            C5Message::error(err + "<br>" + jsonOut + "<br>" + jsonIn);
            return false;
        }
    }

    QString id = C5Database::uuid();
    db[":f_id"] = id;
    db[":f_hallid"] = 0;
    db[":f_prefix"] = "";
    db[":f_state"] = 0;
    db[":f_hall"] = C5Config::defaultHall();
    db[":f_table"] = C5Config::defaultTable();
    db[":f_dateopen"] = fDateOpen;
    db[":f_dateclose"] = QDate::currentDate();
    db[":f_datecash"] = QDate::currentDate();
    db[":f_timeopen"] = fTimeOpen;
    db[":f_timeclose"] = QTime::currentTime();
    db[":f_staff"] = 1;
    db[":f_comment"] = "";
    db[":f_print"] = 1;
    db[":f_amountTotal"] = ui->leTotal->getDouble();
    db[":f_amountCash"] = ui->leTotal->getDouble() - ui->leCard->getDouble();
    db[":f_amountCard"] = ui->leCard->getDouble();
    db[":f_amountBank"] = 0;
    db[":f_amountOther"] = 0;
    db[":f_amountService"] = 0;
    db[":f_amountDiscount"] = 0;
    db[":f_serviceFactor"] = 0;
    db[":f_discountFactor"] = 0;
    db[":f_creditCardId"] = 0;
    db[":f_otherId"] = 0;
    db.insert("o_header", false);
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        db[":f_id"] = C5Database::uuid();
        db[":f_header"] = id;
        db[":f_store"] = C5Config::defaultStore();
        db[":f_goods"] = ui->tblGoods->getInteger(i, 0);
        db[":f_qty"] = ui->tblGoods->getDouble(i, 2);
        db[":f_price"] = ui->tblGoods->getDouble(i, 4);
        db[":f_total"] = ui->tblGoods->getDouble(i, 5);
        db[":f_tax"] = (int) tax;
        db.insert("o_goods", false);
    }
    db.commit();
    return true;
}

void WOrder::changeQty()
{
    int row = ui->tblGoods->currentRow();
    if (row < 0) {
        return;
    }
    double qty = DQty::getQty(this);
    if (qty < 0.001) {
        return;
    }
    ui->tblGoods->setDouble(row, 2, qty);
    ui->tblGoods->setDouble(row, 5, ui->tblGoods->getDouble(row, 4) * qty);
    countTotal();
}

void WOrder::changePrice()
{
    int row = ui->tblGoods->currentRow();
    if (row < 0) {
        return;
    }
    double qty = DQty::getQty(this);
    if (qty < 0.001) {
        return;
    }
    ui->tblGoods->setDouble(row, 4, qty);
    ui->tblGoods->setDouble(row, 5, ui->tblGoods->getDouble(row, 2) * qty);
    countTotal();
}

void WOrder::removeRow()
{
    int row = ui->tblGoods->currentRow();
    if (row < 0) {
        return;
    }
    ui->tblGoods->removeRow(row);
    countTotal();
}

void WOrder::nextRow()
{
    int row = ui->tblGoods->currentRow();
    if (row + 1 < ui->tblGoods->rowCount()) {
        ui->tblGoods->setCurrentItem(ui->tblGoods->item(row + 1, 0));
    }
}

void WOrder::prevRow()
{
    int row = ui->tblGoods->currentRow();
    if (row - 1 > -1) {
        ui->tblGoods->setCurrentItem(ui->tblGoods->item(row - 1, 0));
    }
}

void WOrder::countTotal()
{
    double total = 0;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        total += ui->tblGoods->getDouble(i, 5);
    }
    ui->leTotal->setDouble(total);
}

void WOrder::on_leCash_textChanged(const QString &arg1)
{
    ui->leChange->setDouble(arg1.toDouble() - ui->leTotal->getDouble());
}

void WOrder::on_leCard_textChanged(const QString &arg1)
{
    if (arg1.toDouble() > ui->leTotal->getDouble()) {
        ui->leCard->setDouble(ui->leTotal->getDouble());
    }
}
