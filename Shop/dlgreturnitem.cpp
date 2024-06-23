#include "dlgreturnitem.h"
#include "ui_dlgreturnitem.h"
#include "c5checkbox.h"
#include "c5storedraftwriter.h"
#include "oheader.h"
#include "ogoods.h"
#include "printreceipt.h"

DlgReturnItem::DlgReturnItem() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgReturnItem)
{
    ui->setupUi(this);
    ui->tblOrder->setColumnWidths(ui->tblOrder->columnCount(), 0, 140, 100, 120, 100, 230, 0);
    ui->tblBody->setColumnWidths(ui->tblBody->columnCount(), 0, 30, 120, 200, 80, 80, 80, 80, 0, 0);
    C5Database db(fDBParams);
    db.exec("select f_id, f_name from o_goods_return_reason");
    ui->cbReason->addItem("", 0);
    while (db.nextRow()) {
        ui->cbReason->addItem(db.getString("f_name"), db.getInt("f_id"));
    }
}

DlgReturnItem::~DlgReturnItem()
{
    delete ui;
}

void DlgReturnItem::on_btnSearchReceiptNumber_clicked()
{
    ui->tblOrder->clearContents();
    ui->tblOrder->setRowCount(0);
    C5Database db(fDBParams);
    db.exec(QString("select o.f_id, o.f_datecash, concat(o.f_prefix, o.f_hallid) as f_receipt, h.f_name as f_hallname,  "
                    "t.f_receiptnumber, concat(u.f_last, ' ', u.f_first) as f_saler, o.f_staff "
                    "from o_header o "
                    "inner join h_halls h on h.f_id=o.f_hall "
                    "left join o_tax t on t.f_id=o.f_id "
                    "left join s_user u on u.f_id=o.f_staff "
                    "where concat(o.f_prefix, o.f_hallid)='%1' and o.f_hall in (%2, 10,2,3,4,5,6,7,8,9) "
                    "order by 2 ").arg(ui->leReceiptNumber->text())
            .arg(__c5config.defaultHall())
            ) ;
    while (db.nextRow()) {
        int r = ui->tblOrder->addEmptyRow();
        for (int i = 0; i < db.columnCount(); i++) {
            ui->tblOrder->setData(r, i, db.getValue(i));
        }
    }
}

void DlgReturnItem::on_tblOrder_cellClicked(int row, int column)
{
    Q_UNUSED(column);
    ui->tblBody->clearContents();
    ui->tblBody->setRowCount(0);
    C5Database db(fDBParams);
    db[":f_header"] = ui->tblOrder->getString(row, 0);
    db.exec("select b.f_id, 0, g.f_scancode, g.f_name, b.f_qty, u.f_name, b.f_price, b.f_total, b.f_goods, b.f_return "
            "from o_goods b "
            "inner join c_goods g on g.f_id=b.f_goods "
            "inner join c_units u on u.f_id=g.f_unit "
            "where b.f_header=:f_header");
    while (db.nextRow()) {
        int r = ui->tblBody->addEmptyRow();
        for (int c = 0; c < ui->tblBody->columnCount(); c++) {
            ui->tblBody->setData(r, c, db.getValue(c));
        }
        ui->tblBody->createCheckbox(r, 1);
        if (db.getInt("f_return") > 0) {
            ui->tblBody->checkBox(r, 1)->setChecked(true);
            ui->tblBody->checkBox(r, 1)->setEnabled(false);
        }
    }
}

void DlgReturnItem::on_btnSearchTax_clicked()
{
    ui->tblOrder->clearContents();
    ui->tblOrder->setRowCount(0);
    C5Database db(fDBParams);
    db.exec(QString("select o.f_id, o.f_datecash, h.f_name as f_hallname, concat(o.f_prefix, o.f_hallid) as f_receipt, "
                    "t.f_receiptnumber, concat(u.f_last, ' ', u.f_first) as f_saler, o.f_staff "
                    "from o_header o "
                    "inner join h_halls h on h.f_id=o.f_hall "
                    "left join o_tax t on t.f_id=o.f_id "
                    "left join s_user u on u.f_id=o.f_staff "
                    "where t.f_receiptnumber='%1' "
                    "order by 2 ").arg(ui->leReceiptNumber->text()));
    while (db.nextRow()) {
        int r = ui->tblOrder->addEmptyRow();
        for (int i = 0; i < db.columnCount(); i++) {
            ui->tblOrder->setData(r, i, db.getValue(i));
        }
    }
}

void DlgReturnItem::on_btnReturn_clicked()
{
    int reason = ui->cbReason->currentData().toInt();
    if (reason == 0) {
        C5Message::error(tr("Reason not selected"));
        return;
    }
    bool ret = false;
    double returnAmount = 0;
    QList<int> rows;
    bool haveStore = false;
    for (int i = 0; i < ui->tblBody->rowCount(); i++) {
        if (ui->tblBody->checkBox(i, 1)->isChecked()) {
            if (ui->tblBody->getInteger(i, 9) > 0) {
                continue;
            }
            ret = true;
            haveStore = true;
            returnAmount += ui->tblBody->getDouble(i, 7);
            rows.append(i);
        }
    }

    C5Database db(fDBParams);
    C5StoreDraftWriter dw(db);
    db[":f_oheader"] = ui->tblOrder->getString(ui->tblOrder->currentRow(), 0);
    db.exec("SELECT sum(e.f_amount) "
            "from a_header_cash a "
            "inner join e_cash e on e.f_header=a.f_id "
            "where a.f_oheader=:f_oheader");
    QMap<int, double> cashmap;
    while (db.nextRow()) {
        cashmap[__c5config.cashId()] = db.getDouble("f_amount");
    }

    if (!ret) {
        C5Message::error(tr("Nothing to return"));
        return;
    }

    OHeader oheader;
    oheader.staff = ui->tblOrder->getInteger(ui->tblOrder->currentRow(), 6) == 0 ? 1 : ui->tblOrder->getInteger(ui->tblOrder->currentRow(), 6);
    oheader.state = ORDER_STATE_CLOSE;
    oheader.amountTotal = returnAmount * -1;
    oheader.amountCash = returnAmount * -1;
    oheader.saleType = SALE_RETURN;
    oheader.hall = __c5config.defaultHall();
    if (!dw.hallId(oheader.prefix, oheader.hallId, __c5config.defaultHall())) {
        C5Message::error(dw.fErrorMsg);
        db.rollback();
        return;
    }
    QString err;
    if (!oheader.write(db, err)) {
        C5Message::error(err);
        db.rollback();
        return;
    }

    QString storeDocComment;
    QString storeDocId;
    QString storedocUserNum;
    QString saledoc = ui->tblOrder->getString(ui->tblOrder->currentRow(), 2);
    if (haveStore) {
        storeDocComment = QString("%1 %2").arg(tr("Return of sale"), saledoc);
        storeDocId;
        storedocUserNum = dw.storeDocNum(DOC_TYPE_STORE_INPUT, __c5config.defaultStore(), true, 0);
        if (!dw.writeAHeader(storeDocId, storedocUserNum, DOC_STATE_DRAFT, DOC_TYPE_STORE_INPUT,
                             oheader.staff, QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0, 0,
                             storeDocComment, 1, __c5config.getValue(param_default_currency).toInt())) {
            C5Message::error(dw.fErrorMsg);
            db.rollback();
            return;
        }
    }

    for (int j = 0; j < rows.count(); j++) {
        int i = rows.at(j);
        QString adraftid;
        double price = 0;
        db[":f_id"] = ui->tblBody->getString(i, 0);
        db.exec("select f_storerec from o_goods where f_id=:f_id");
        if (db.nextRow()) {
            db[":f_id"] = db.getString(0);
            db.exec("select f_price from a_store_draft where f_id=:f_id");
            if (db.nextRow()) {
                price = db.getDouble(0);
            }
        }
        if (haveStore) {
            db[":f_scancode"] = ui->tblBody->getString(i, 2);
            db.exec("select f_storeid from c_goods where f_scancode=:f_scancode");
            db.nextRow();
            int storeid = db.getInt("f_storeid");
            if (!dw.writeAStoreDraft(adraftid, storeDocId, __c5config.defaultStore(), 1,
                                     storeid, ui->tblBody->getDouble(i, 4),
                                     price, price * ui->tblBody->getDouble(i, 4),
                                     DOC_REASON_SALE_RETURN, adraftid, i + 1, "")) {
                C5Message::error(dw.fErrorMsg);
                db.rollback();
                return;
            }
        }
        OGoods g;
        g.header = oheader._id();
        g.goods = ui->tblBody->getInteger(i, 8);
        g.qty = ui->tblBody->getDouble(i, 4);
        g.price = ui->tblBody->getDouble(i, 6);
        g.total = ui->tblBody->getDouble(i, 7);
        g.sign = -1;
        g.row = i + 1;
        g.storeRec = adraftid;
        g.returnFrom = ui->tblBody->getString(i, 0);
        g.tax = ui->tblOrder->getInteger(ui->tblOrder->currentRow(), 4);
        if (!g.write(db, err)) {
            C5Message::error(err);
            db.rollback();
            return;
        }
        if (!dw.updateField("o_goods", "f_return", reason, "f_id", ui->tblBody->getString(i, 0))) {
            C5Message::error(dw.fErrorMsg);
            db.rollback();
            return;
        }
        if (!dw.updateField("o_goods", "f_returnfrom", g.id, "f_id", ui->tblBody->getString(i, 0))) {
            C5Message::error(dw.fErrorMsg);
            db.rollback();
            return;
        }
        if (!dw.updateField("o_header", "f_comment", QString("%1 %2").arg(tr("Return from"), saledoc), "f_id", oheader.id)) {
            C5Message::error(dw.fErrorMsg);
            db.rollback();
            return;
        }
        on_tblOrder_cellClicked(ui->tblOrder->currentRow(), 0);
    }

    /* STORE CASH DOC */
    if (haveStore) {
        QString fCashUuid, fCashRowId;
        QString fCashUserId = QString("%1").arg(dw.counterAType(DOC_TYPE_CASH), C5Config::docNumDigitsInput(), 10, QChar('0'));
        QString purpose = tr("Return of sale") + " " + saledoc;
        dw.writeAHeader(fCashUuid, fCashUserId, DOC_STATE_DRAFT, DOC_TYPE_CASH, oheader.staff, QDate::currentDate(),
                        QDate::currentDate(), QTime::currentTime(), 0, returnAmount, purpose, 1, __c5config.getValue(param_default_currency).toInt());
        dw.writeAHeaderCash(fCashUuid, 0, __c5config.cashId(), 1, storeDocId, "", 0);
        dw.writeECash(fCashRowId, fCashUuid, __c5config.cashId(), -1, purpose, returnAmount, fCashRowId, 1);
        if (!dw.writeAHeaderStore(storeDocId, oheader.staff, oheader.staff, "", QDate::currentDate(), __c5config.defaultStore(), 0, 1, fCashUuid, 0, 0, oheader._id())) {
            C5Message::error(dw.fErrorMsg);
            db.rollback();
            return;
        }
    }

    int cashid = ui->btnReturnCash->isChecked() ? __c5config.cashId() : __c5config.nocashId();
        QString cashdocid;
        double cashamount = returnAmount;
        int counter = dw.counterAType(DOC_TYPE_CASH);
        if (counter == 0) {
            C5Message::error(dw.fErrorMsg);
            db.rollback();
            return;
        }
        if (!dw.writeAHeader(cashdocid, QString::number(counter), DOC_STATE_SAVED, DOC_TYPE_CASH, oheader.staff, QDate::currentDate(),
                             QDate::currentDate(), QTime::currentTime(), 0, cashamount, tr("Return of") + " " + saledoc,
                             1, __c5config.getValue(param_default_currency).toInt())) {
            C5Message::error(dw.fErrorMsg);
            db.rollback();
            return;
        }
        if (!dw.writeAHeaderCash(cashdocid, 0, cashid, 1, "", oheader._id(), 0)) {
            C5Message::error(dw.fErrorMsg);
            db.rollback();
            return;
        }
        QString cashUUID;
        if (!dw.writeECash(cashUUID, cashdocid, cashid, -1, saledoc, cashamount, cashUUID, 1)) {
            C5Message::error(dw.fErrorMsg);
            db.rollback();
            return;
        }


    if (haveStore) {
        if (dw.writeInput(storeDocId, err)) {
            if (!dw.writeAHeader(storeDocId, storedocUserNum, DOC_STATE_SAVED, DOC_TYPE_STORE_INPUT, oheader.staff, QDate::currentDate(),
                                 QDate::currentDate(), QTime::currentTime(), 0, 0, storeDocComment,
                                 1, __c5config.getValue(param_default_currency).toInt())) {
                C5Message::error(dw.fErrorMsg);
                db.rollback();
                return;
            }
            dw.writeTotalStoreAmount(storeDocId);
        }
    }

    if (!C5Config::localReceiptPrinter().isEmpty()) {
        PrintReceipt p;
        p.print(oheader._id(), db);
    }

    db.commit();
    C5Message::info(tr("Return completed"));
}

void DlgReturnItem::on_btnReturnCash_clicked()
{
    ui->btnReturnCash->setChecked(true);
    ui->btnReturnCard->setChecked(false);
}

void DlgReturnItem::on_btnReturnCard_clicked()
{
    ui->btnReturnCash->setChecked(false);
    ui->btnReturnCard->setChecked(true);
}
