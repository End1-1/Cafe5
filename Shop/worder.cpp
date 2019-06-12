#include "worder.h"
#include "ui_worder.h"
#include "c5database.h"
#include "c5config.h"
#include "printtaxn.h"
#include "dqty.h"
#include "c5utils.h"
#include "c5printing.h"

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
    QString id = C5Database::uuid();
    QString sn, firm, address, fiscal, hvhh, rseq, devnum, time;

    if (tax) {
        PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(), this);
        for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
            pt.addGoods(ui->tblGoods->getString(i, 6), //dep
                        ui->tblGoods->getString(i, 7), //adg
                        ui->tblGoods->getString(i, 0), //goods id
                        ui->tblGoods->getString(i, 1), //name
                        ui->tblGoods->getDouble(i, 4), //price
                        ui->tblGoods->getDouble(i, 2), 0); //qty
        }
        QString jsonIn, jsonOut, err;
        int result = 0;
        result = pt.makeJsonAndPrint(ui->leCard->getDouble(), 0, jsonIn, jsonOut, err);

        db[":f_order"] = id;
        db[":f_date"] = QDate::currentDate();
        db[":f_time"] = QTime::currentTime();
        db[":f_in"] = jsonIn;
        db[":f_out"] = jsonOut;
        db[":f_err"] = err;
        db[":f_result"] = result;
        db.insert("o_tax_log", false);
        QSqlQuery q(db.fDb);
        if (result == pt_err_ok) {
            PrintTaxN::parseResponse(jsonOut, firm, hvhh, fiscal, rseq, sn, address, devnum, time);
            db[":f_id"] = id;
            db.exec("delete from o_tax where f_id=:f_id");
            db[":f_id"] = id;
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
            pt.saveTimeResult(id, q);
        } else {
            C5Message::error(err + "<br>" + jsonOut + "<br>" + jsonIn);
            pt.saveTimeResult("Not saved - " + id, q);
            return false;
        }
    }

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
        db[":f_tax"] = tax ? 1 : 0;
        db.insert("o_goods", false);
    }
    db.commit();
    if (!C5Config::localReceiptPrinter().isEmpty()) {
        QFont font(qApp->font());
        font.setPointSize(20);
        C5Printing p;
        p.setSceneParams(650, 2800, QPrinter::Portrait);
        p.setFont(font);
        if (tax) {
            p.ltext(firm, 0);
            p.br();
            p.ltext(address, 0);
            p.br();
            p.ltext(tr("Department"), 0);
            p.rtext(ui->tblGoods->getString(0, 6));
            p.br();
            p.ltext(tr("Tax number"), 0);
            p.rtext(hvhh);
            p.br();
            p.ltext(tr("Device number"), 0);
            p.rtext(devnum);
            p.br();
            p.ltext(tr("Serial"), 0);
            p.rtext(sn);
            p.br();
            p.ltext(tr("Fiscal"), 0);
            p.rtext(fiscal);
            p.br();
            p.ltext(tr("Receipt number"), 0);
            p.rtext(rseq);
            p.br();
            p.ltext(tr("Date"), 0);
            p.rtext(time);
            p.br();
            p.ltext(tr("(F)"), 0);
            p.br();
        }
        p.br(2);
        p.setFontBold(true);
        p.ctext(tr("Class | Name | Qty | Price | Total"));
        p.setFontBold(false);
        p.br();
        p.line(3);
        p.br(3);
        for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
            if (tax) {
                p.ltext(QString("%1 %2, %3").arg(tr("Class:"))
                        .arg(ui->tblGoods->getString(i, 7))
                        .arg(ui->tblGoods->getString(i, 1)), 0);
            } else {
                p.ltext(QString("%1").arg(ui->tblGoods->getString(i, 1)), 0);
            }
            p.br();
            p.ltext(QString("%1 X %2 = %3")
                    .arg(float_str(ui->tblGoods->getDouble(i, 2), 2))
                    .arg(ui->tblGoods->getDouble(i, 4), 2)
                    .arg(float_str(ui->tblGoods->getDouble(i, 2) * ui->tblGoods->getDouble(i, 4), 2)), 0);
            p.br();
            p.line();
            p.br(2);
        }
        p.line(4);
        p.br(3);
        p.setFontBold(true);
        p.ltext(tr("Need to pay"), 0);
        p.rtext(float_str(ui->leTotal->getDouble(), 2));
        p.br();
        p.br();

        p.line();
        p.br();

        if (ui->leCash->getDouble() > 0.001) {
            p.ltext(tr("Payment, cash"), 0);
            p.rtext(float_str(ui->leCash->getDouble(), 2));
        }
        if (ui->leCard->getDouble() > 0.001) {
            p.ltext(tr("Payment, card"), 0);
            p.rtext(float_str(ui->leCard->getDouble(), 2));
        }

        p.setFontSize(20);
        p.setFontBold(true);
        p.br(p.fLineHeight * 3);
        p.ltext(tr("Thank you for visit!"), 0);
        p.br();
        p.ltext(tr("Printed"), 0);
        p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
        p.br();
        p.print(C5Config::localReceiptPrinter(), QPrinter::Custom);
    }
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
