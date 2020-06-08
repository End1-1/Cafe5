#include "vieworder.h"
#include "ui_vieworder.h"
#include "c5config.h"
#include "c5message.h"
#include "goodsreturnreason.h"
#include "c5database.h"
#include "c5utils.h"
#include "c5checkbox.h"
#include "c5storedraftwriter.h"
#include "printreceipt.h"

ViewOrder::ViewOrder(const QString &order) :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::ViewOrder)
{
    ui->setupUi(this);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 30, 300, 100, 100, 100, 0, 200);
    C5Database db(__c5config.replicaDbParams());
    db[":f_id"] = order;
    db.exec("select * from o_header where f_id=:f_id");
    if (db.nextRow()) {
        ui->leAmount->setDouble(db.getDouble("f_amounttotal"));
        fSaleType = db.getInt("f_saletype");
        fPartner = db.getInt("f_partner");
        fSaleDoc = QString("%1%2, %3").arg(db.getString("f_prefix")).arg(db.getString("f_hallid")).arg(db.getDate("f_datecash").toString(FORMAT_DATE_TO_STR));
    } else {
        C5Message::error(tr("Document is not exists"));
        return;
    }
    db[":f_header"] = order;
    db.exec("select b.f_id, g.f_name, g.f_id as f_goodsid, b.f_qty, b.f_price, b.f_total, f_scancode,  "
            "g.f_service, b.f_return, b.f_tax "
            "from o_goods b "
            "inner join c_goods g on g.f_id=b.f_goods "
            "where b.f_header=:f_header");
    while (db.nextRow()) {
        int r = ui->tbl->addEmptyRow();
        ui->tbl->setString(r, 0, db.getString("f_id"));
        ui->tbl->createCheckbox(r, 1);
        ui->tbl->setString(r, 2, db.getString("f_name"));
        ui->tbl->setString(r, 3, db.getString("f_qty"));
        ui->tbl->setString(r, 4, db.getString("f_price"));
        ui->tbl->setString(r, 5, db.getString("f_total"));
        ui->tbl->setString(r, 6, db.getString("f_goodsid"));
        ui->tbl->setString(r, 7, db.getString("f_scancode"));
        ui->tbl->setString(r, 8, db.getString("f_service"));
        ui->tbl->setInteger(r, 9, db.getInt("f_return"));
        if (db.getInt("f_return") > 0 || db.getDouble("f_price") < 0) {
            ui->tbl->checkBox(r, 1)->setEnabled(false);
        }
        ui->leTaxNumber->setText(db.getString("f_tax"));
    }
    if (ui->leAmount->getDouble() < 0) {
        ui->btnReturn->setVisible(false);
    }
}

ViewOrder::~ViewOrder()
{
    delete ui;
}

void ViewOrder::on_btnReturn_clicked()
{
    GoodsReturnReason *r = new GoodsReturnReason();
    r->exec();
    int reason = r->fReason;
    delete r;
    if (reason == 0) {
        return;
    }
    bool ret = false;
    double returnAmount = 0;
    QList<int> rows;
    bool haveStore = false;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        if (ui->tbl->checkBox(i, 1)->isChecked()) {
            if (ui->tbl->getDouble(i, 4) < 0) {
                ui->tbl->checkBox(i, 1)->setChecked(false);
                continue;
            }
            if (ui->tbl->getInteger(i, 9) == 1) {
                continue;
            }
            if (ui->tbl->getInteger(i, 8) == 0) {
                haveStore = true;
            }
            ret = true;
            returnAmount += ui->tbl->getDouble(i, 5);
            rows.append(i);
        }
    }
    if (!ret) {
        C5Message::error(tr("Nothing to return"));
        return;
    }
    C5Database db(__c5config.replicaDbParams());
    C5StoreDraftWriter dw(db);
    db.startTransaction();
    QString headerPrefix;
    int headerId;
    if (!dw.hallId(headerPrefix, headerId, __c5config.defaultHall().toInt())) {
        return returnFalse(dw.fErrorMsg, &db);
    }
    QString oheaderid;
    if (!dw.writeOHeader(oheaderid, headerId, headerPrefix, ORDER_STATE_CLOSE, __c5config.defaultHall().toInt(), __c5config.defaultTable(), QDate::currentDate(), QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), QTime::currentTime(), __userid, "", 1, returnAmount * -1, returnAmount * -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, fSaleType, fPartner)) {
        return returnFalse(dw.fErrorMsg, &db);
    }

    QString storeDocComment;
    QString storeDocId;
    QString storedocUserNum;
    if (haveStore) {
        storeDocComment = QString("%1 %2").arg(tr("Return of sale")).arg(fSaleDoc);
        storeDocId;
        storedocUserNum = dw.storeDocNum(DOC_TYPE_STORE_INPUT, __c5config.defaultStore(), true, 0);
        if (!dw.writeAHeader(storeDocId, storedocUserNum, DOC_STATE_DRAFT, DOC_TYPE_STORE_INPUT, __userid, QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0, 0, storeDocComment, 0, 0)) {
            return returnFalse(dw.fErrorMsg, &db);
        }
    }

    for (int j = 0; j < rows.count(); j++) {
        int i = rows.at(j);
        QString ogoodsid;
        QString adraftid;
        if (haveStore && ui->tbl->getInteger(i, 8) == 0) {
            if (!dw.writeAStoreDraft(adraftid, storeDocId, __c5config.defaultStore(), 1, ui->tbl->getInteger(i, 6), ui->tbl->getDouble(i, 3), ui->tbl->getDouble(i, 4), ui->tbl->getDouble(i, 5), DOC_REASON_SALE_RETURN, adraftid, i + 1)) {
                return returnFalse(dw.fErrorMsg, &db);
            }
        }
        if (!dw.writeOGoods(ogoodsid, oheaderid, "", __c5config.defaultStore(), ui->tbl->getInteger(i, 6), ui->tbl->getDouble(i, 3), ui->tbl->getDouble(i, 4) * -1,  ui->tbl->getDouble(i, 5) * -1, ui->leTaxNumber->getInteger(), 1, i + 1, adraftid, 0, 0, reason)) {
            return returnFalse(dw.fErrorMsg, &db);
        }
        if (!dw.updateField("o_goods", "f_return", reason, "f_id", ui->tbl->getString(i, 0))) {
            return returnFalse(dw.fErrorMsg, &db);
        }
        ui->tbl->setInteger(i, 9, 1);
        ui->tbl->checkBox(i, 1)->setEnabled(false);
        ui->tbl->checkBox(i, 1)->setChecked(false);
        ui->tbl->setDouble(i, 4, ui->tbl->getDouble(i, 4) * -1);
    }

    /* STORE CASH DOC */
    if (haveStore) {
        QString fCashUuid, fCashRowId;
        QString fCashUserId = QString("%1").arg(dw.counterAType(DOC_TYPE_CASH), C5Config::docNumDigitsInput(), 10, QChar('0'));
        QString purpose = tr("Return of sale") + " " + fSaleDoc;
        dw.writeAHeader(fCashUuid, fCashUserId, DOC_STATE_DRAFT, DOC_TYPE_CASH, __userid, QDate::currentDate(),
                        QDate::currentDate(), QTime::currentTime(), 0, returnAmount,
                        purpose, 0, 0);
        dw.writeAHeaderCash(fCashUuid, 0, __c5config.cashId(), 1, storeDocId, "");
        dw.writeECash(fCashRowId, fCashUuid, __c5config.cashId(), -1, purpose, returnAmount, fCashRowId, 1);
        if (!dw.writeAHeaderStore(storeDocId, __userid, __userid, "", QDate(), __c5config.defaultStore(), 0, 1, fCashUuid, 0, 0)) {
            return returnFalse(dw.fErrorMsg, &db);
        }
    }

    QString cashdocid;
    int counter = dw.counterAType(DOC_TYPE_CASH);
    if (counter == 0) {
        return returnFalse(dw.fErrorMsg, &db);
    }
    if (!dw.writeAHeader(cashdocid, QString::number(counter), DOC_STATE_SAVED, DOC_TYPE_CASH, __userid, QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0, returnAmount, tr("Return of") + " " + fSaleDoc, 0, 0)) {
        return returnFalse(dw.fErrorMsg, &db);
    }
    if (!dw.writeAHeaderCash(cashdocid, 0, __c5config.cashId(), 1, "", oheaderid)) {
        return returnFalse(dw.fErrorMsg, &db);
    }
    QString cashUUID;
    if (!dw.writeECash(cashUUID, cashdocid, __c5config.cashId(), -1, fSaleDoc, returnAmount, cashUUID, 1)) {
        return returnFalse(dw.fErrorMsg, &db);
    }

    QString err;
    if (haveStore) {
        if (dw.writeInput(storeDocId, err)) {
            if (!dw.writeAHeader(storeDocId, storedocUserNum, DOC_STATE_SAVED, DOC_TYPE_STORE_INPUT, __userid, QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0, 0, storeDocComment, 0, 0)) {
                return returnFalse(dw.fErrorMsg, &db);
            }
            dw.writeTotalStoreAmount(storeDocId);
        }
    }

    if (!C5Config::localReceiptPrinter().isEmpty()) {
        PrintReceipt p;
        p.print(oheaderid, db);
    }

    db.commit();
    C5Message::info(tr("Return completed"));
}

void ViewOrder::on_pushButton_2_clicked()
{
    reject();
}

void ViewOrder::returnFalse(const QString &msg, C5Database *db)
{
    C5Message::error(msg);
    db->rollback();
    db->close();
}
