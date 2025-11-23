#include "dlgreturnitem.h"
#include "ui_dlgreturnitem.h"
#include "c5checkbox.h"
#include "c5storedraftwriter.h"
#include "oheader.h"
#include "c5config.h"
#include "c5utils.h"
#include "c5lineedit.h"
#include "ogoods.h"
#include "printreceipt.h"
#include "printtaxn.h"
#include "c5message.h"
#include "jsons.h"
#include <QJsonArray>

DlgReturnItem::DlgReturnItem() :
    C5Dialog(),
    ui(new Ui::DlgReturnItem)
{
    ui->setupUi(this);
    ui->tblOrder->setColumnWidths(ui->tblOrder->columnCount(), 0, 140, 100, 120, 100, 230, 0, 200);
    ui->tblBody->setColumnWidths(ui->tblBody->columnCount(), 0, 30, 120, 200, 80, 80, 80, 80, 0, 150, 150);
    ui->wexchange->setVisible(false);
    ui->lbPartner->setVisible(false);
}

DlgReturnItem::~DlgReturnItem()
{
    delete ui;
}

void DlgReturnItem::setMode(int mode)
{
    fMode = mode;

    if(mode == 2) {
        ui->btnReturn->setText(tr("Exchange"));
        ui->wexchange->setVisible(true);
    }
}

void DlgReturnItem::checkQtyResponse(const QJsonObject &jdoc)
{
    Q_UNUSED(jdoc);
    fHttp->httpQueryFinished(sender());
}

void DlgReturnItem::on_btnSearchReceiptNumber_clicked()
{
    ui->leBarcode->setText(ui->leBarcode->text().trimmed());

    if(fMode == 2) {
        if(ui->leBarcode->text().isEmpty()) {
            C5Message::error(tr("Enter barcode"));
            return;
        }
    }

    ui->tblOrder->clearContents();
    ui->tblOrder->setRowCount(0);
    C5Database db;
    db.exec(QString(R"(select o.f_id, o.f_datecash, concat(o.f_prefix, o.f_hallid) as f_receipt,
                    h.f_name as f_hallname,
                    t.f_receiptnumber, concat(u.f_last, ' ', u.f_first) as f_saler, o.f_staff,
                    p.f_taxname
                    from o_header o
                    inner join h_halls h on h.f_id=o.f_hall
                    left join o_tax t on t.f_id=o.f_id
                    left join s_user u on u.f_id=o.f_staff
                    left join c_partners p on p.f_id=o.f_partner
                    where concat(o.f_prefix, o.f_hallid)='%1' and o.f_hall in (%2, 10,2,3,4,5,6,7,8,9)
                    and DATEDIFF(CURRENT_DATE, f_datecash)<15
                    order by 2 )").arg(ui->leReceiptNumber->text())
            .arg(__c5config.defaultHall())
           ) ;

    while(db.nextRow()) {
        int r = ui->tblOrder->addEmptyRow();

        for(int i = 0; i < db.columnCount(); i++) {
            ui->tblOrder->setData(r, i, db.getValue(i));

            if(db.getDate("f_datecash").daysTo(QDate::currentDate()) > 14) {
                ui->tblOrder->item(r, 0)->setData(Qt::BackgroundRole, QVariant::fromValue(Qt::red));
            }
        }

        //ui->tblOrder->setData(r, )
        if(db.getDate("f_datecash").daysTo(QDate::currentDate()) > 14) {
            ui->tblOrder->item(r, 0)->setData(Qt::UserRole, 14);
        }
    }
}

void DlgReturnItem::on_tblOrder_cellClicked(int row, int column)
{
    Q_UNUSED(column);
    ui->tblBody->clearContents();
    ui->tblBody->setRowCount(0);
    C5Database db;
    int d14 = ui->tblOrder->item(row, 0)->data(Qt::UserRole).toInt();

    if(d14 > 0) {
        C5Message::error(tr("You cannot return this item"));
        return;
    }

    fHttp->createHttpQueryLambda("/engine/v2/shop/shop-order/get",
    QJsonObject{{"id", ui->tblOrder->getString(row, 0)}}, [this](const QJsonObject & jdoc) {
        QJsonArray jbody = jdoc["body"].toArray();
        QJsonObject jheader = jdoc["header"].toObject();

        for(int i = 0; i < jbody.size(); i++) {
            const QJsonObject jt = jbody.at(i).toObject();
            int r = ui->tblBody->addEmptyRow();
            ui->tblBody->setData(r, 0, jt["f_id"].toString());
            ui->tblBody->setData(r, 1, jt["f_x"].toInt());
            ui->tblBody->setData(r, 2, jt["f_scancode"].toString());
            ui->tblBody->setData(r, 3, jt["f_name"].toString());
            ui->tblBody->setData(r, 4, jt["f_qty"].toDouble());
            ui->tblBody->setData(r, 5, jt["f_unit"].toString());
            ui->tblBody->setData(r, 6, jt["f_price"].toDouble());
            ui->tblBody->setData(r, 7, jt["f_total"].toDouble());
            ui->tblBody->setData(r, 8, jt["f_goods"].toInt());
            ui->tblBody->setData(r, 9, jt["f_return"].toInt());
            ui->tblBody->setData(r, 9, jt["f_returnedqty"].toDouble());
            auto *l = ui->tblBody->createLineEdit(r, 10);
            l->setEnabled(__c5config.fMainJson["change_qty_return_items"].toBool());
            l->setDouble(0);
            connect(l, &C5LineEdit::textChanged, this, [this, l](const QString & arg) {
                int r, c;

                if(ui->tblBody->findWidget(l, r, c)) {
                    if(l->getDouble() > ui->tblBody->getDouble(r, 4) - ui->tblBody->getDouble(r, 9)) {
                        l->setDouble(ui->tblBody->getDouble(r, 4) - ui->tblBody->getDouble(r, 9));
                    }
                }

                countReturnAmount();
            });
            auto *cb = ui->tblBody->createCheckbox(r, 1);
            connect(cb, &C5CheckBox::clicked, this, [this, cb](bool checked) {
                int r, c;

                if(ui->tblBody->findWidget(cb, r, c)) {
                    C5LineEdit *l = ui->tblBody->lineEdit(r, 10);
                    l->setDouble(checked ? ui->tblBody->getDouble(r, 4) - ui->tblBody->getDouble(r, 9) : 0);
                }

                countReturnAmount();
            });

            if(jt["f_returnedqty"].toDouble() - jt["f_qty"].toDouble() <  0.001 && jt["f_returnedqty"].toDouble() > 0.001) {
                ui->tblBody->checkBox(r, 1)->setChecked(true);
                ui->tblBody->checkBox(r, 1)->setEnabled(false);
            }

            if(ui->leBarcode->text().isEmpty() == false) {
                if(ui->leBarcode->text() != jt["f_scancode"].toString()) {
                    cb->setEnabled(false);
                }
            }
        }

        ui->cbPaymentType->setEnabled(__c5config.fMainJson["change_qty_return_items"].toBool());

        if(jheader["f_amountcash"].toDouble() > 0.01) {
            ui->cbPaymentType->setCurrentIndex(0);
        } else {
            ui->cbPaymentType->setCurrentIndex(1);
        }

        QJsonObject jpartner = jdoc["partner"].toObject();
        ui->lbPartner->setProperty("id", jpartner["f_id"].toInt());
        ui->lbPartner->setProperty("debt", jpartner["f_amountdebt"].toDouble());

        if(ui->lbPartner->property("debt").toDouble() > 0.001) {
            ui->cbPaymentType->setCurrentIndex(2);
        }

        if(!jpartner.isEmpty()) {
            ui->lbPartner->setText(QString("%1 (%2)").arg(jpartner["f_taxname"].toString(), float_str(jpartner["debt"].toDouble(), 2)));
            ui->lbPartner->setVisible(true);
        }
    }, [](const QJsonObject & jerr) {});
}

void DlgReturnItem::on_btnSearchTax_clicked()
{
    ui->tblOrder->clearContents();
    ui->tblOrder->setRowCount(0);
    C5Database db;
    db.exec(QString("select o.f_id, o.f_datecash, h.f_name as f_hallname, concat(o.f_prefix, o.f_hallid) as f_receipt, "
                    "t.f_receiptnumber, concat(u.f_last, ' ', u.f_first) as f_saler, o.f_staff "
                    "from o_header o "
                    "inner join h_halls h on h.f_id=o.f_hall "
                    "left join o_tax t on t.f_id=o.f_id "
                    "left join s_user u on u.f_id=o.f_staff "
                    "where t.f_receiptnumber='%1' "
                    "order by 2 ").arg(ui->leReceiptNumber->text()));

    while(db.nextRow()) {
        int r = ui->tblOrder->addEmptyRow();

        for(int i = 0; i < db.columnCount(); i++) {
            ui->tblOrder->setData(r, i, db.getValue(i));
        }
    }
}

void DlgReturnItem::on_btnReturn_clicked()
{
    int reason = 1;

    if(ui->leReturnAmount->getDouble() < 0.01) {
        C5Message::error(tr("Nothing to return"));
        return;
    }

    ui->btnReturn->setEnabled(false);
    QString headerid = ui->tblOrder->getString(ui->tblOrder->currentRow(), 0);
    QMap<QString, QVariant> oldAmountsMap;
    C5Database db;
    db[":f_id"] = ui->tblOrder->getString(ui->tblOrder->currentRow(), 0);
    db.exec("select * from o_header where f_id=:f_id");
    db.nextRow();
    db.rowToMap(oldAmountsMap);
    //FISCAL RETURN
    db[":f_id"] = headerid;
    db.exec("select * from o_tax_log where f_order=:f_id and f_state=1");

    if(db.nextRow() && ui->cbPaymentType->currentIndex() != 2) {
        QJsonObject jout = __strjson(db.getString("f_out"));
        QString crn = jout["crn"].toString();
        int rseq = jout["rseq"].toInt();
        PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(),
                     C5Config::taxCashier(), C5Config::taxPin(), this);

        for(int i = 0; i < ui->tblBody->rowCount(); i++) {
            auto *l = ui->tblBody->lineEdit(i, 10);

            if(l->getDouble() < 0.001) {
                continue;
            }

            pt.addReturnItem(i, l->getDouble());
        }

        QString jsnin, jsnout, err;
        int result;

        if(oldAmountsMap["f_amountcash"].toDouble() > 0) {
            pt.fCashAmountForReturn = ui->leReturnAmount->getDouble();
        } else if(oldAmountsMap["f_amountcard"].toDouble() > 0 || oldAmountsMap["f_amountidram"].toDouble() > 0) {
            pt.fCardAmountForReturn = ui->leReturnAmount->getDouble();
        } else if(oldAmountsMap["f_amountprepaid"].toDouble() > 0) {
            pt.fPrepaymentAmountForReturn = ui->leReturnAmount->getDouble();
        }

        result = pt.printTaxback(rseq, crn, jsnin, jsnout, err);
        db[":f_id"] = db.uuid();
        db[":f_order"] = ui->tblOrder->getString(0, 0);
        db[":f_date"] = QDate::currentDate();
        db[":f_time"] = QTime::currentTime();
        db[":f_in"] = jsnin;
        db[":f_out"] = jsnout;
        db[":f_err"] = err;
        db[":f_result"] = result;
        db.insert("o_tax_log", false);

        if(result != pt_err_ok) {
            C5Message::error(err);
            ui->btnReturn->setEnabled(true);
            return;
        } else {
            db[":f_fiscal"] = QVariant();
            db[":f_receiptnumber"] = QVariant();
            db[":f_time"] = QVariant();
            db.update("o_tax", "f_id", headerid);
        }
    }

    //END OF FISCAL RETURN
    int cashid = __c5config.cashId();

    if(oldAmountsMap["f_amountcard"].toDouble() > 0.1
            || oldAmountsMap["f_amountidram"].toDouble() > 0.1
            || oldAmountsMap["f_amounttelcell"].toDouble() > 0.1
            || oldAmountsMap["f_amountbank"].toDouble() > 0.1) {
        cashid = __c5config.nocashId();
    }

    C5StoreDraftWriter dw(db);
    db[":f_oheader"] = headerid;
    db.exec("SELECT sum(e.f_amount) "
            "from a_header_cash a "
            "inner join e_cash e on e.f_header=a.f_id "
            "where a.f_oheader=:f_oheader");
    QMap<int, double> cashmap;

    while(db.nextRow()) {
        cashmap[__c5config.cashId()] = db.getDouble("f_amount");
    }

    OHeader oheader;
    oheader.staff = ui->tblOrder->getInteger(ui->tblOrder->currentRow(),
                    6) == 0 ? 1 : ui->tblOrder->getInteger(ui->tblOrder->currentRow(), 6);
    oheader.state = ORDER_STATE_CLOSE;
    oheader.amountTotal = ui->leReturnAmount->getDouble() * -1;
    oheader.amountCash = cashid == __c5config.cashId() ? ui->leReturnAmount->getDouble() * -1 : 0;
    oheader.amountCard = cashid == __c5config.nocashId() ? ui->leReturnAmount->getDouble() * -1 : 0;
    oheader.saleType = SALE_RETURN;
    oheader.hall = __c5config.defaultHall();

    if(!dw.hallId(oheader.prefix, oheader.hallId, __c5config.defaultHall())) {
        C5Message::error(dw.fErrorMsg);
        ui->btnReturn->setEnabled(true);
        return;
    }

    QString err;

    if(!oheader.write(db, err)) {
        C5Message::error(err);
        ui->btnReturn->setEnabled(true);
        return;
    }

    //WRITE STORE INPUT DOCUMENT
    QString storeDocComment;
    QString storeDocId;
    QString storedocUserNum;
    QString saledoc = ui->tblOrder->getString(ui->tblOrder->currentRow(), 2);
    storeDocComment = QString("%1 %2").arg(tr("Return of sale"), saledoc);
    storedocUserNum = dw.storeDocNum(DOC_TYPE_STORE_INPUT, __c5config.defaultStore(), true, 0);

    if(!dw.writeAHeader(storeDocId, storedocUserNum, DOC_STATE_DRAFT, DOC_TYPE_STORE_INPUT,
                        oheader.staff, QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0, 0,
                        storeDocComment, 1, __c5config.getValue(param_default_currency).toInt())) {
        C5Message::error(dw.fErrorMsg);
        ui->btnReturn->setEnabled(true);
        return;
    }

    for(int j = 0; j < ui->tblBody->rowCount(); j++) {
        auto *l = ui->tblBody->lineEdit(j, 10);

        if(l->getDouble() < 0.001) {
            continue;
        }

        QString adraftid;
        double price = 0;
        db[":f_id"] = ui->tblBody->getString(j, 0);
        db.exec("select f_storerec from o_goods where f_id=:f_id");

        if(db.nextRow()) {
            db[":f_id"] = db.getString(0);
            db.exec("select f_price from a_store_draft where f_id=:f_id");

            if(db.nextRow()) {
                price = db.getDouble(0);
            }
        }

        db[":f_scancode"] = ui->tblBody->getString(j, 2);
        db.exec("select f_storeid from c_goods where f_scancode=:f_scancode");
        db.nextRow();
        int storeid = db.getInt("f_storeid");

        if(!dw.writeAStoreDraft(adraftid, storeDocId, __c5config.defaultStore(), 1,
                                storeid, l->getDouble(),
                                price, price * l->getDouble(),
                                DOC_REASON_SALE_RETURN, adraftid, j + 1, "")) {
            C5Message::error(dw.fErrorMsg);
            ui->btnReturn->setEnabled(true);
            return;
        }

        OGoods g;
        g.header = oheader._id();
        g.goods = ui->tblBody->getInteger(j, 8);
        g.qty = l->getDouble();
        g.price = ui->tblBody->getDouble(j, 6);
        g.total = g.qty * g.price;
        g.sign = -1;
        g.row = j + 1;
        g.storeRec = adraftid;
        g.returnFrom = ui->tblBody->getString(j, 0);
        g.tax = ui->tblOrder->getInteger(ui->tblOrder->currentRow(), 4);

        if(!g.write(db, err)) {
            C5Message::error(err);
            ui->btnReturn->setEnabled(true);
            return;
        }

        if(!dw.updateField("o_goods", "f_return", reason, "f_id", ui->tblBody->getString(j, 0))) {
            C5Message::error(dw.fErrorMsg);
            ui->btnReturn->setEnabled(true);
            return;
        }

        if(!dw.updateField("o_goods", "f_returnedqty", l->getDouble() + ui->tblBody->getDouble(j, 9), "f_id", ui->tblBody->getString(j, 0))) {
            C5Message::error(dw.fErrorMsg);
            ui->btnReturn->setEnabled(true);
            return;
        }

        if(!dw.updateField("o_goods", "f_returnfrom", g.id, "f_id", ui->tblBody->getString(j, 0))) {
            C5Message::error(dw.fErrorMsg);
            ui->btnReturn->setEnabled(true);
            return;
        }

        if(!dw.updateField("o_header", "f_comment", QString("%1 %2").arg(tr("Return from"), saledoc), "f_id", oheader.id)) {
            C5Message::error(dw.fErrorMsg);
            ui->btnReturn->setEnabled(true);
            return;
        }
    }

    //CHECK PARTNER DEBT
    if(ui->lbPartner->property("id").toInt() > 0 && ui->lbPartner->property("debt").toDouble() > 0) {
        db[":f_order"] = oheader.id;
        db[":f_date"] = QDate::currentDate();
        db[":f_costumer"] = ui->lbPartner->property("id").toInt();
        db[":f_amount"] = ui->leReturnAmount->getDouble();
        db[":f_currency"] = 1;
        db[":f_source"] = 1;
        db.insert("b_clients_debts", false);
    }

    // STORE CASH DOC
    QString fCashUuid, fCashRowId;
    QString fCashUserId = QString("%1").arg(dw.counterAType(DOC_TYPE_CASH), C5Config::docNumDigitsInput(), 10, QChar('0'));
    QString purpose = tr("Return of sale") + " " + saledoc;
    dw.writeAHeader(fCashUuid, fCashUserId, DOC_STATE_DRAFT, DOC_TYPE_CASH, oheader.staff, QDate::currentDate(),
                    QDate::currentDate(), QTime::currentTime(), 0, ui->leReturnAmount->getDouble(), purpose, 1,
                    __c5config.getValue(param_default_currency).toInt());
    dw.writeAHeaderCash(fCashUuid, 0, __c5config.cashId(), 1, storeDocId, "");
    dw.writeECash(fCashRowId, fCashUuid, __c5config.cashId(), -1, purpose, ui->leReturnAmount->getDouble(), fCashRowId, 1);

    if(!dw.writeAHeaderStore(storeDocId, oheader.staff, oheader.staff, "",
                             QDate::currentDate(), __c5config.defaultStore(),
                             0, 1, fCashUuid, 0, 0, oheader._id())) {
        C5Message::error(dw.fErrorMsg);
        ui->btnReturn->setEnabled(true);
        return;
    }

    //WRITE CASH DOC IF THIS IS A NOT DEBT
    if(ui->lbPartner->property("debt").toDouble() < 1) {
        QString cashdocid;
        double cashamount = ui->leReturnAmount->getDouble();
        int counter = dw.counterAType(DOC_TYPE_CASH);

        if(counter == 0) {
            C5Message::error(dw.fErrorMsg);
            ui->btnReturn->setEnabled(true);
            return;
        }

        if(!dw.writeAHeader(cashdocid, QString::number(counter), DOC_STATE_SAVED, DOC_TYPE_CASH, oheader.staff,
                            QDate::currentDate(),
                            QDate::currentDate(), QTime::currentTime(), 0, cashamount, tr("Return of") + " " + saledoc,
                            1, __c5config.getValue(param_default_currency).toInt())) {
            C5Message::error(dw.fErrorMsg);
            ui->btnReturn->setEnabled(true);
            return;
        }

        if(!dw.writeAHeaderCash(cashdocid, 0, cashid, 1, "", oheader._id())) {
            C5Message::error(dw.fErrorMsg);
            ui->btnReturn->setEnabled(true);
            return;
        }

        QString cashUUID;

        if(!dw.writeECash(cashUUID, cashdocid, cashid, -1, saledoc, cashamount, cashUUID, 1)) {
            C5Message::error(dw.fErrorMsg);
            ui->btnReturn->setEnabled(true);
            return;
        }
    }

    if(dw.writeInput(storeDocId, err)) {
        if(!dw.writeAHeader(storeDocId, storedocUserNum, DOC_STATE_SAVED, DOC_TYPE_STORE_INPUT,
                            oheader.staff,
                            QDate::currentDate(),
                            QDate::currentDate(), QTime::currentTime(), 0, 0, storeDocComment,
                            1, __c5config.getValue(param_default_currency).toInt())) {
            C5Message::error(dw.fErrorMsg);
            ui->btnReturn->setEnabled(true);
            return;
        }

        dw.writeTotalStoreAmount(storeDocId);
    }

    if(!C5Config::localReceiptPrinter().isEmpty()) {
        PrintReceipt p;
        p.print(oheader._id(), db);
    }

    C5Message::info(tr("Return completed"));
    accept();
}

void DlgReturnItem::on_leExchange_returnPressed()
{
    fHttp->createHttpQuery("/shop/check-qty.php", QJsonObject{
        {"store", __c5config.defaultStore()},
        {"barcode", ui->leExchange->text()}},
    SLOT(checkQtyResponse(QJsonObject)));
}

void DlgReturnItem::countReturnAmount()
{
    double totalAmount = 0;

    for(int i = 0; i < ui->tblBody->rowCount(); i++) {
        auto *ch = ui->tblBody->checkBox(i, 1);
        auto *l = ui->tblBody->lineEdit(i, 10);

        if(ch->isChecked()) {
            double qtyTotalReturn = l->getDouble();

            if(qtyTotalReturn > 0) {
                totalAmount += qtyTotalReturn * ui->tblBody->getDouble(i, 6);
            }
        }
    }

    ui->leReturnAmount->setDouble(totalAmount);
}

void DlgReturnItem::on_btnClose_clicked()
{
    accept();
}
