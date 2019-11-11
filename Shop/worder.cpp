#include "worder.h"
#include "ui_worder.h"
#include "c5database.h"
#include "c5config.h"
#include "printtaxn.h"
#include "dqty.h"
#include "c5message.h"
#include "c5permissions.h"
#include "c5printing.h"
#include "printreceipt.h"
#include "working.h"
#include "c5utils.h"
#include "c5storedraftwriter.h"
#include <QInputDialog>

WOrder::WOrder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WOrder)
{
    ui->setupUi(this);
    fWorking = static_cast<Working*>(parent);
    fDateOpen = QDate::currentDate();
    fTimeOpen = QTime::currentTime();
    ui->tblGoods->setColumnWidths(ui->tblGoods->columnCount(), 0, 300, 80, 80, 80, 80, 80, 80);
    ui->lbRefund->setVisible(false);
    fModeRefund = false;
    fCostumerId = 0;
    ui->lbDisc->setVisible(false);
    ui->leDisc->setVisible(false);
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
    if (fModeRefund) {
        tax = false;
    }

    if (ui->tblGoods->rowCount() == 0) {
        return false;
    }
    if (C5Message::question(tr("Write order?")) != QDialog::Accepted) {
        return false;
    }
    C5Database db(C5Config::dbParams());
    db.startTransaction();
    QString id = C5Database::uuid();
    int discId = 0;
    QString sn, firm, address, fiscal, hvhh, rseq, devnum, time;

    if (fCostumerId > 0) {
        db[":f_type"] = fCardMode;
        db[":f_value"] = fCardValue;
        db[":f_card"] = fCardId;
        db[":f_data"] = 0;
        db[":f_order"] = id;
        discId = db.insert("b_history");
    }

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
        QSqlQuery *q = new QSqlQuery(db.fDb);
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
            pt.saveTimeResult(id, *q);
            delete q;
        } else {
            C5Message::error(err + "<br>" + jsonOut + "<br>" + jsonIn);
            pt.saveTimeResult("Not saved - " + id, *q);
            delete q;
            return false;
        }
    }

    int sign = fModeRefund ? -1 : 1;

    db[":f_id"] = C5Config::defaultHall();
    db.exec("select f_counter + 1, f_prefix as f_counter from h_halls where f_id=:f_id for update");
    QString pref, hallid;
    if (db.nextRow()) {
        hallid = db.getString(0);
        pref = db.getString(1);
        db[":f_counter"] = db.getInt(0);
        db.update("h_halls", where_id(C5Config::defaultHall()));
    }

    db[":f_id"] = id;
    db[":f_hallid"] = hallid.toInt();
    db[":f_prefix"] = pref;
    db[":f_state"] = ORDER_STATE_CLOSE;
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
    db[":f_amountTotal"] = ui->leTotal->getDouble() * sign;
    db[":f_amountCash"] = (ui->leTotal->getDouble() - ui->leCard->getDouble()) * sign;
    db[":f_amountCard"] = ui->leCard->getDouble() * sign;
    db[":f_amountBank"] = 0;
    db[":f_amountOther"] = 0;
    db[":f_amountService"] = 0;
    db[":f_amountDiscount"] = 0;
    db[":f_serviceMode"] = 0;
    db[":f_serviceFactor"] = 0;
    db[":f_discountFactor"] = 0;
    db[":f_creditCardId"] = 0;
    db[":f_otherId"] = 0;
    if (db.insert("o_header", false) == 0) {
        C5Message::error(db.fLastError);
        db.rollback();
        db.close();
        return false;
    }
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        db[":f_id"] = C5Database::uuid();
        db[":f_header"] = id;
        db[":f_store"] = C5Config::defaultStore();
        db[":f_goods"] = ui->tblGoods->getInteger(i, 0);
        db[":f_qty"] = ui->tblGoods->getDouble(i, 2);
        db[":f_price"] = ui->tblGoods->getDouble(i, 4);
        db[":f_total"] = ui->tblGoods->getDouble(i, 5);
        db[":f_tax"] = tax ? rseq.toInt() : 0;
        db[":f_sign"] = sign;
        db[":f_taxdept"] = C5Config::taxDept();
        db[":f_row"] = i + 1;
        if (db.insert("o_goods", false) == 0) {
            db.rollback();
            db.close();
            return false;
        }
    }

    C5StoreDraftWriter dw(db);
    if (fModeRefund) {
        if (!dw.writeFromShopInput(QDate::currentDate(), id)) {
            C5Message::error(dw.fErrorMsg);
            db.rollback();
            return false;
        }
    } else {
        if (!dw.writeFromShopOutput(QDate::currentDate(), id)) {
            C5Message::error(dw.fErrorMsg);
            db.rollback();
            return false;
        }
    }

    if (!C5Config::localReceiptPrinter().isEmpty()) {
        PrintReceipt p;
        p.print(id, db);
    }

    fCostumerId = 0;
    db.commit();
    return true;
}

void WOrder::fixCostumer(const QString &code)
{
    C5Database db(C5Config::dbParams());
    db[":f_code"] = code;
    db.exec("select * from b_cards_discount where f_code=:f_code");
    if (!db.nextRow()) {
        return;
    }
    if (QDate::currentDate() > db.getDate("f_dateend")) {
        C5Message::error(tr("Cards was expired"));
        return;
    }
    fCostumerId = db.getInt("f_client");
    fCardId = db.getInt("f_id");
    fCardMode = db.getInt("f_mode");
    fCardValue = db.getDouble("f_value");
    db[":f_id"] = fCostumerId;
    db.exec("select * from b_clients where f_id=:f_id");
    if (!db.nextRow()) {
        return;
    }
    fWorking->markDiscount(db.getString("f_firstname") + " " + db.getString("f_lastname"));
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

void WOrder::refund()
{
    if (!fModeRefund) {
        if (!fWorking->getAdministratorRights()) {
            return;
        }
    }
    fModeRefund = !fModeRefund;
    ui->lbRefund->setVisible(fModeRefund);
}

void WOrder::setDiscount(const QString &label, const QString &value)
{
    if (label.isEmpty()) {
        ui->leDisc->setVisible(false);
        ui->lbDisc->setVisible(false);
    } else {
        ui->leDisc->setVisible(true);
        ui->lbDisc->setVisible(true);
        ui->lbDisc->setText(label);
        ui->leDisc->setText(value);
    }
    countTotal();
}

void WOrder::countTotal()
{
    double total = 0;
    double discount = 0;
    double disc = 0;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        double amount = ui->tblGoods->getDouble(i, 5);
        switch (fCardMode) {
        case CARD_TYPE_DISCOUNT:
            disc = amount * fCardValue;
            discount += disc;
            total += amount - disc;
            break;
        case CARD_TYPE_ACCUMULATIVE:
            disc = amount * fCardValue;
            discount += disc;
            total += amount;
            break;
        case CARD_TYPE_COUNT_ORDER:
            total += amount;
            break;
        case CARD_TYPE_MANUAL:
            total += amount;
            break;
        default:
            total += ui->tblGoods->getDouble(i, 5);
            break;
        }
    }
    switch (fCardMode) {
    case CARD_TYPE_DISCOUNT: {
        ui->leDisc->setText(float_str(discount, 2));
        ui->lbDisc->setText(QString("%1 %%2").arg(tr("Discount")).arg(float_str(fCardValue, 2)));
        break;
    }
    case CARD_TYPE_ACCUMULATIVE: {
        ui->leDisc->setText(float_str(discount, 2));
        ui->lbDisc->setText(QString("%1 %%2").arg(tr("Discount")).arg(float_str(fCardValue, 2)));
        break;
    }
    case CARD_TYPE_COUNT_ORDER: {
        ui->lbDisc->setText(QString("%1").arg(tr("Visit counter")));
        break;
    }
    case CARD_TYPE_MANUAL:
        ui->lbDisc->setText(QString("%1").arg(tr("Discount")).arg(float_str(fCardValue, 2)));
        break;
    default:
        break;
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
