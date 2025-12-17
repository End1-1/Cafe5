#include "vieworder.h"
#include "ui_vieworder.h"
#include "jsons.h"
#include "c5config.h"
#include "c5message.h"
#include "goodsreturnreason.h"
#include "c5database.h"
#include "c5utils.h"
#include "c5user.h"
#include "c5checkbox.h"
#include "c5storedraftwriter.h"
#include "printreceipt.h"
#include "printtaxn.h"
#include "printreceiptgroup.h"
#include "worder.h"
#include "selectprinters.h"
#include "dlggetidname.h"
#include "dqty.h"
#include <QClipboard>
#include <QJsonObject>
#include "c5printrecipta4.h"
#include "dlgdate.h"
#include "working.h"
#include "c5printing.h"
#include "ogoods.h"
#include <QPrinter>
#include <QPrintDialog>

ViewOrder::ViewOrder(Working *w, const QString &order, C5User *user) :
    C5ShopDialog(user),
    ui(new Ui::ViewOrder),
    fWorking(w)
{
    ui->setupUi(this);
    fUuid = order;
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 30, 300, 100, 100, 100, 0, 200);
    C5Database db;
    db[":f_id"] = order;
    db.exec("select o.*, concat(' ', u.f_last, u.f_first) as f_saler, "
            "concat_ws(' ', p.f_taxcode, p.f_taxname, p.f_contact, p.f_phone) as f_buyer "
            "from o_header o "
            "left join s_user u on u.f_id=o.f_staff "
            "left join c_partners p on p.f_id=o.f_partner "
            "where o.f_id=:f_id");

    if(db.nextRow()) {
        ui->leOrderNum->setText(QString("%1%2").arg(db.getString("f_prefix")).arg(db.getInt("f_hallid")));
        ui->leAmount->setDouble(db.getDouble("f_amounttotal"));
        fOHeader.saleType = db.getInt("f_saletype");
        fOHeader.partner = db.getInt("f_partner");
        fSaleDoc = QString("%1%2, %3").arg(db.getString("f_prefix"),
                                           db.getString("f_hallid"),
                                           db.getDate("f_datecash").toString(FORMAT_DATE_TO_STR));
        ui->leDate->setDate(db.getDate("f_datecash"));
        ui->leTime->setText(db.getTime("f_timeclose").toString("HH:mm"));
        ui->leUUID->setText(db.getString("f_id"));
        ui->leCash->setDouble(db.getDouble("f_amountcash"));
        ui->leCard->setDouble(db.getDouble("f_amountcard"));
        ui->leIdram->setDouble(db.getDouble("f_amountidram"));
        ui->leDebt->setDouble(db.getDouble("f_amountdebt"));
        ui->leBank->setDouble(db.getDouble("f_amountbank"));
        ui->leTelcell->setDouble(db.getDouble("f_amounttelcell"));
        ui->leSaler->setText(db.getString("f_saler"));
        ui->leBuyer->setText(db.getString("f_buyer"));
    } else {
        C5Message::error(tr("Document is not exists"));
        return;
    }

    db[":f_id"] = order;
    db.exec("select concat(' ', u.f_last, u.f_first) as f_deliveryman "
            "from o_header_options o "
            "left join s_user u on u.f_id=o.f_deliveryman "
            "where o.f_id=:f_id");

    if(db.nextRow()) {
        ui->leDeliveryMan->setText(db.getString("f_deliveryman"));
    }

    db[":f_header"] = order;
    db.exec("select b.f_id, g.f_name, g.f_id as f_goodsid, b.f_qty, b.f_price, b.f_total, f_scancode,  "
            "g.f_service, b.f_return, t.f_receiptnumber as f_tax, b.f_store "
            "from o_goods b "
            "left join o_header h on h.f_id=b.f_header "
            "left join o_tax t on t.f_id=h.f_id "
            "inner join c_goods g on g.f_id=b.f_goods "
            "where b.f_header=:f_header "
            "order by b.f_row ");

    while(db.nextRow()) {
        int r = ui->tbl->addEmptyRow();
        ui->tbl->setString(r, 0, db.getString("f_id"));
        ui->tbl->createCheckbox(r, 1);
        ui->tbl->setString(r, 2, db.getString("f_name"));
        ui->tbl->setDouble(r, 3, db.getDouble("f_qty"));
        ui->tbl->setDouble(r, 4, db.getDouble("f_price"));
        ui->tbl->setDouble(r, 5, db.getDouble("f_total"));
        ui->tbl->setInteger(r, 6, db.getInt("f_goodsid"));
        ui->tbl->setString(r, 7, db.getString("f_scancode"));
        ui->tbl->setString(r, 8, db.getString("f_service"));
        ui->tbl->setInteger(r, 9, db.getInt("f_return"));
        ui->tbl->setInteger(r, 10, db.getInt("f_store"));

        if(db.getInt("f_return") > 0 || db.getDouble("f_price") < 0) {
            ui->tbl->checkBox(r, 1)->setEnabled(false);
        }

        ui->leTaxNumber->setText(db.getString("f_tax"));
        ui->btnPrintFiscal->setVisible(ui->leTaxNumber->getInteger() == 0);
        ui->btnTaxReturn->setVisible(!ui->btnPrintFiscal->isVisible());
    }

    if(ui->leAmount->getDouble() < 0) {
        ui->btnReturn->setVisible(false);
    }

    ui->btnSaveReturn->setVisible(false);
}

ViewOrder::~ViewOrder()
{
    delete ui;
}

void ViewOrder::removeOrderResponse(const QJsonObject &jdoc)
{
    qDebug() << sender();
    fHttp->httpQueryFinished(sender());
    fWorking->openDraft(jdoc["id"].toString());
    close();
}

void ViewOrder::on_btnReturn_clicked()
{
    if(ui->leDate->date().daysTo(QDate::currentDate()) > 14) {
        C5Message::error(tr("You cannot return this item"));
        return;
    }

    GoodsReturnReason r;
    r.exec();
    int reason = r.fReason;
    ui->leReturnReason->setProperty("reason", r.fReason);
    ui->leReturnReason->setText(r.fReasonName);

    if(reason == 0) {
        return;
    }

    setProperty("return", true);
    setStyleSheet("background:rgb(255, 210, 217);");
    style()->polish(this);
    ui->btnSaveReturn->setVisible(true);
    ui->leUUID->setProperty("returnfrom", ui->leUUID->text());
    ui->leUUID->clear();
    ui->leDate->setDate(QDate::currentDate());
    ui->btnReturn->setEnabled(false);

    for(int i = 0; i < ui->tbl->rowCount(); i++) {
        ui->tbl->item(i, 3)->setData(Qt::UserRole, ui->tbl->getDouble(i, 3));
    }
}

void ViewOrder::returnFalse(const QString &msg, C5Database *db)
{
    Q_UNUSED(db);
    C5Message::error(msg);
}

void ViewOrder::countOrder()
{
    double total = 0;

    for(int i = 0; i < ui->tbl->rowCount(); i++) {
        ui->tbl->setDouble(i, 5, ui->tbl->getDouble(i, 3) *ui->tbl->getDouble(i, 4));
        total += ui->tbl->getDouble(i, 5);
    }

    ui->leAmount->setDouble(total);
}

void ViewOrder::on_btnTaxReturn_clicked()
{
    C5Database db;
    db[":f_id"] = ui->leUUID->text();
    db.exec("select * from o_tax_log where f_order=:f_id and f_state=1");

    if(!db.nextRow()) {
        C5Message::error(tr("No fiscal exists for this order"));
        return;
    }

    QJsonObject jout = __strjson(db.getString("f_out"));
    QString crn = jout["crn"].toString();
    QString rseq = ui->leTaxNumber->text();
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(),
                 C5Config::taxCashier(), C5Config::taxPin(), this);
    QString jsnin, jsnout, err;
    int result;
    result = pt.printTaxback(rseq.toInt(), crn, jsnin, jsnout, err);
    db[":f_id"] = db.uuid();
    db[":f_order"] = fUuid;
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db[":f_in"] = jsnin;
    db[":f_out"] = jsnout;
    db[":f_err"] = err;
    db[":f_result"] = result;
    db.insert("o_tax_log", false);

    if(result != pt_err_ok) {
        C5Message::error(err);
    } else {
        db[":f_fiscal"] = QVariant();
        db[":f_receiptnumber"] = QVariant();
        db[":f_time"] = QVariant();
        db.update("o_tax", "f_id", fUuid);
        C5Message::info(tr("Taxback complete"));
    }
}

void ViewOrder::on_btnClose_clicked()
{
    accept();
}

void ViewOrder::on_btnEditDate_clicked()
{
    QDate d;

    if(DlgDate::getDate(d)) {
        if(C5Message::question(tr("Confirm to change date")) != QDialog::Accepted) {
            return;
        }

        C5Database db;
        db[":f_datecash"] = d;

        if(db.update("o_header", "f_id", ui->leUUID->text())) {
            ui->leDate->setDate(d);
        }
    }
}

void ViewOrder::on_btnCopyUUID_clicked()
{
    qApp->clipboard()->setText(ui->leUUID->text());
}

void ViewOrder::on_btnEditDeliveryMan_clicked()
{
    QString id, name;

    if(DlgGetIDName::get(__c5config.dbParams(), id, name, idname_users_fullname, this) == false) {
        return;
    }

    if(C5Message::question(tr("Confirm to change the deliveryman")) != QDialog::Accepted) {
        return;
    }

    C5Database db;
    db[":f_deliveryman"] = id;

    if(db.update("o_header_options", "f_id", ui->leUUID->text())) {
        ui->leDeliveryMan->setText(name);
    }
}

void ViewOrder::on_btnEditSaler_clicked()
{
    QString id, name;

    if(DlgGetIDName::get(__c5config.dbParams(), id, name, idname_users_fullname, this) == false) {
        return;
    }

    if(C5Message::question(tr("Confirm to change the saler")) != QDialog::Accepted) {
        return;
    }

    C5Database db;
    db[":f_staff"] = id;

    if(db.update("o_header", "f_id", ui->leUUID->text())) {
        ui->leSaler->setText(name);
    }
}

void ViewOrder::on_btnEditBuyer_clicked()
{
    QString id, name;

    if(DlgGetIDName::get(__c5config.dbParams(), id, name, idname_partners_full, this) == false) {
        return;
    }

    if(C5Message::question(tr("Confirm to change the buyer")) != QDialog::Accepted) {
        return;
    }

    C5Database db;
    db[":f_partner"] = id;

    if(db.update("o_header", "f_id", ui->leUUID->text())) {
        ui->leBuyer->setText(name);
    }
}

void ViewOrder::on_btnPrintReceipt_clicked()
{
    if(!C5Config::localReceiptPrinter().isEmpty()) {
        C5Database db;
        PrintReceiptGroup p;

        switch(C5Config::shopPrintVersion()) {
        case 1: {
            bool p1, p2;

            if(SelectPrinters::selectPrinters(p1, p2)) {
                if(p1) {
                    p.print(ui->leUUID->text(), db, 1);
                }

                if(p2) {
                    p.print(ui->leUUID->text(), db, 2);
                }
            }

            break;
        }

        case 2:
            p.print2(ui->leUUID->text(), db);
            break;

        default:
            break;
        }
    }
}

void ViewOrder::on_btnPrintFiscal_clicked()
{
    if(C5Message::question(tr("Confirm to print fiscal")) != QDialog::Accepted) {
        return;
    }

    QString rseq;
    C5Database db;

    if(printCheckWithTax(db, ui->leUUID->text(), rseq)) {
        ui->leTaxNumber->setText(rseq);
    }
}

bool ViewOrder::printCheckWithTax(C5Database &db, const QString &id, QString &rseq)
{
    QElapsedTimer et;
    et.start();
    bool resultb = true;
    db[":f_id"] = id;
    db.exec("select * from o_tax where f_id=:f_id");

    if(db.nextRow()) {
        if(db.getInt("f_receiptnumber") > 0) {
            C5Message::error(tr("Cannot print tax twice"));
            return resultb;
        }
    }

    db[":f_id"] = id;
    db.exec("select * from o_header where f_id=:f_id");
    db.nextRow();
    double card = db.getDouble("f_amountcard");
    double idram = db.getDouble("f_idram");
    int partner = db.getInt("f_partner");
    QString useExtPos = idram > 0.01 ? "true" : C5Config::taxUseExtPos();
    QString partnerHvhh;

    if(partner > 0) {
        db[":f_id"] = partner;
        db.exec("select f_taxcode from c_partners where f_id=:f_id");

        if(db.nextRow()) {
            partnerHvhh = db.getString(0);
        }
    }

    db[":f_id"] = id;
    db.exec("select og.f_id, og.f_goods, g.f_name, og.f_qty, "
            "gu.f_name as f_unitname, og.f_price, og.f_total,"
            "t.f_taxdept, t.f_adgcode, "
            "og.f_store "
            "from o_goods og "
            "left join c_goods g on g.f_id=og.f_goods "
            "left join c_units gu on gu.f_id=g.f_unit "
            "left join c_groups t on t.f_id=g.f_group "
            "where og.f_header=:f_id "
            "order by og.f_row ");
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), useExtPos, C5Config::taxCashier(),
                 C5Config::taxPin(), 0);
    pt.fPartnerTin = partnerHvhh;

    while(db.nextRow()) {
        pt.addGoods(db.getInt("f_taxdept"), //dep
                    db.getString("f_adgcode"), //adg
                    db.getString("f_goods"), //goods id
                    db.getString("f_name"), //name
                    db.getDouble("f_price"), //price
                    db.getDouble("f_qty"), //qty
                    db.getDouble("f_discountfactor") * 100); //discount
    }

    QString jsonIn, jsonOut, err;
    int result = 0;
    result = pt.makeJsonAndPrint(card, 0, jsonIn, jsonOut, err);
    QJsonObject jtax;
    jtax["f_order"] = id;
    jtax["f_elapsed"] = et.elapsed();
    jtax["f_in"] = QJsonDocument::fromJson(jsonIn.toUtf8()).object();
    jtax["f_out"] = QJsonDocument::fromJson(jsonOut.toUtf8()).object();
    jtax["f_err"] = err;
    jtax["f_result"] = result;
    jtax["f_state"] = result == pt_err_ok ? 1 : 0;
    db.exec(QString("call sf_create_shop_tax('%1')").arg(QString(QJsonDocument(jtax).toJson(QJsonDocument::Compact))));

    if(result == pt_err_ok) {
        jtax = QJsonDocument::fromJson(jsonOut.toUtf8()).object();
        rseq = QString::number(jtax["rseq"].toInt());
        C5Message::info(tr("Printed"));
    } else {
        C5Message::error(err);
    }

    return resultb;
}

void ViewOrder::on_btnPrintReceiptA4_clicked()
{
    C5PrintReciptA4 p(ui->leUUID->text(), this);
    QString err;

    if(!p.print(err)) {
        C5Message::error(err);
    }
}

void ViewOrder::on_btnMakeDraft_clicked()
{
    if(ui->leDate->date().daysTo(QDate::currentDate()) > 14) {
        C5Message::error(tr("You cannot return this item"));
        return;
    }

    if(ui->leDate->date().daysTo(QDate::currentDate()) > 14) {
        C5Message::error(tr("You cannot return this item"));
        return;
    }

    if(C5Message::question(tr("Confirm to make draft")) != QDialog::Accepted) {
        return;
    }

    fHttp->createHttpQuery("/engine/shop/make-draft.php", QJsonObject{{"id", ui->leUUID->text()}}, SLOT(
        removeOrderResponse(QJsonObject)));
}

void ViewOrder::on_btnSaveReturn_clicked()
{
    if(ui->leReturnReason->property("reason").toInt() == 0) {
        C5Message::error(tr("he return reason must be specified."));
        return;
    }

    int uid = 1;
    C5Database db;
    C5StoreDraftWriter dw(db);
    bool ret = false;
    double returnAmount = 0;
    QList<int> rows;
    bool haveStore = false;

    for(int i = 0; i < ui->tbl->rowCount(); i++) {
        if(!ui->tbl->checkBox(i, 1)->isChecked()) {
            continue;
        }

        if(ui->tbl->getDouble(i, 3) < 0.001) {
            continue;
        }

        if(ui->tbl->getInteger(i, 9) == 1) {
            continue;
        }

        if(ui->tbl->getInteger(i, 8) == 0) {
            haveStore = true;
        }

        ret = true;
        returnAmount += ui->tbl->getDouble(i, 5);
        rows.append(i);
    }

    if(!ret) {
        C5Message::error(tr("Nothing to return"));
        return;
    }

    db[":f_oheader"] = fUuid;
    db.exec("SELECT a.f_cashin, e.f_amount "
            "from a_header_cash a "
            "inner join e_cash e on e.f_header=a.f_id "
            "where a.f_oheader=:f_oheader");

    if(db.rowCount() > 1) {
        if(rows.count() != ui->tbl->rowCount()) {
            C5Message::info(tr("Mixed payment mode. Only full return available."));
            return;
        }
    }

    QMap<int, double> cashmap;

    while(db.nextRow()) {
        cashmap[db.getInt("f_cashin")] = db.getDouble("f_amount");
    }

    if(!ret) {
        C5Message::error(tr("Nothing to return"));
        return;
    }

    QString err;
    OHeader oheader;

    if(!dw.hallId(oheader.prefix, oheader.hallId, __c5config.defaultHall())) {
        return returnFalse(dw.fErrorMsg, &db);
    }

    oheader.state = ORDER_STATE_CLOSE;
    oheader.cashier = mUser->id();
    oheader.hall = __c5config.defaultHall();
    oheader.table = __c5config.defaultTable();
    oheader.staff = uid;
    oheader.amountTotal = returnAmount * -1;
    oheader.amountCash = ui->leCash->getDouble() > 0.1 ? returnAmount * -1 : 0;
    oheader.amountCard = ui->leCard->getDouble() > 0.1 ? returnAmount * -1 : 0;
    oheader.saleType = SALE_RETURN;
    oheader.partner = fOHeader.partner;
    oheader.source = 2;

    if(!oheader.write(db, err)) {
        return returnFalse(err, &db);
    }

    QString storeDocComment;
    QString storeDocId;
    QString storedocUserNum;

    if(haveStore) {
        storeDocComment = QString("%1 %2").arg(tr("Return of sale"), fSaleDoc);
        storedocUserNum = dw.storeDocNum(DOC_TYPE_STORE_INPUT, __c5config.defaultStore(), true, 0);

        if(!dw.writeAHeader(storeDocId, storedocUserNum, DOC_STATE_DRAFT, DOC_TYPE_STORE_INPUT,
                            uid, QDate::currentDate(), QDate::currentDate(), QTime::currentTime(), 0, 0,
                            storeDocComment, 1, __c5config.getValue(param_default_currency).toInt())) {
            return returnFalse(dw.fErrorMsg, &db);
        }
    }

    for(int j = 0; j < rows.count(); j++) {
        int i = rows.at(j);
        QString adraftid;
        double price = 0;
        db[":f_id"] = ui->tbl->getString(i, 0);
        db.exec("select f_storerec from o_goods where f_id=:f_id");

        if(db.nextRow()) {
            db[":f_id"] = db.getString(0);
            db.exec("select f_price from a_store_draft where f_id=:f_id");

            if(db.nextRow()) {
                price = db.getDouble(0);
            }
        }

        if(haveStore && ui->tbl->getInteger(i, 8) == 0) {
            db[":f_id"] = ui->tbl->getInteger(i, 6);
            db.exec("select f_storeid from c_goods where f_id=:f_id");
            db.nextRow();
            int storeid = db.getInt("f_storeid");

            if(!dw.writeAStoreDraft(adraftid, storeDocId, __c5config.defaultStore(), 1,
                                    storeid, ui->tbl->getDouble(i, 3),
                                    price, price * ui->tbl->getDouble(i, 3),
                                    DOC_REASON_SALE_RETURN, adraftid, i + 1, "")) {
                return returnFalse(dw.fErrorMsg, &db);
            }
        }

        OGoods g;
        g.header = oheader._id();
        g.store = __c5config.defaultStore();
        g.goods = ui->tbl->getInteger(i, 6);
        g.qty = ui->tbl->getDouble(i, 3);
        g.price = ui->tbl->getDouble(i, 4);
        g.total = g.qty * g.price;
        g.tax = ui->leTaxNumber->getInteger();
        g.sign = -1;
        g.taxDept = 1;
        g.row = i + 1;
        g.storeRec = adraftid;
        g.return_ = ui->leReturnReason->property("reason").toInt();
        g.returnFrom =  ui->tbl->getString(i, 0);

        if(!g.write(db, err)) {
            return returnFalse(err, &db);
        }

        if(!dw.updateField("o_goods", "f_return", ui->leReturnReason->property("reason").toInt(), "f_id",
                           ui->tbl->getString(i, 0))) {
            return returnFalse(dw.fErrorMsg, &db);
        }

        // if (!dw.updateField("o_goods", "f_returnfrom", ogoodsid, "f_id", ui->tbl->getString(i, 0) )) {
        //     return returnFalse(dw.fErrorMsg, &db);
        // }
        if(!dw.updateField("o_header", "f_comment", QString("%1 %2").arg(tr("Return from")).arg(ui->leOrderNum->text()),
                           "f_id", oheader.id.toString())) {
            return returnFalse(dw.fErrorMsg, &db);
        }

        ui->tbl->setInteger(i, 9, 1);
        ui->tbl->checkBox(i, 1)->setEnabled(false);
        ui->tbl->checkBox(i, 1)->setChecked(false);
        ui->tbl->setDouble(i, 4, ui->tbl->getDouble(i, 4) * -1);
    }

    /* STORE CASH DOC */
    if(haveStore) {
        QString fCashUuid, fCashRowId;
        QString fCashUserId = QString("%1").arg(dw.counterAType(DOC_TYPE_CASH), C5Config::docNumDigitsInput(), 10, QChar('0'));
        QString purpose = tr("Return of sale") + " " + fSaleDoc;
        dw.writeAHeader(fCashUuid, fCashUserId, DOC_STATE_DRAFT, DOC_TYPE_CASH, uid, QDate::currentDate(),
                        QDate::currentDate(), QTime::currentTime(), 0, returnAmount, purpose,
                        1, __c5config.getValue(param_default_currency).toInt());
        dw.writeAHeaderCash(fCashUuid, 0, __c5config.cashId(), 1, storeDocId, "");
        dw.writeECash(fCashRowId, fCashUuid, __c5config.cashId(), -1, purpose, returnAmount, fCashRowId, 1);

        if(!dw.writeAHeaderStore(storeDocId, uid, uid, "", QDate::currentDate(), __c5config.defaultStore(), 0, 1, fCashUuid, 0,
                                 0, oheader._id())) {
            return returnFalse(dw.fErrorMsg, &db);
        }
    }

    for(QMap<int, double>::const_iterator it = cashmap.constBegin(); it != cashmap.constEnd(); it++) {
        QString cashdocid;
        double cashamount = cashmap.count() == 1 ? returnAmount : it.value();
        int counter = dw.counterAType(DOC_TYPE_CASH);

        if(counter == 0) {
            return returnFalse(dw.fErrorMsg, &db);
        }

        if(!dw.writeAHeader(cashdocid, QString::number(counter), DOC_STATE_SAVED, DOC_TYPE_CASH, uid, QDate::currentDate(),
                            QDate::currentDate(), QTime::currentTime(), 0, cashamount, tr("Return of") + " " + fSaleDoc,
                            1, __c5config.getValue(param_default_currency).toInt())) {
            return returnFalse(dw.fErrorMsg, &db);
        }

        if(!dw.writeAHeaderCash(cashdocid, 0, it.key(), 1, "", oheader._id())) {
            return returnFalse(dw.fErrorMsg, &db);
        }

        QString cashUUID;

        if(!dw.writeECash(cashUUID, cashdocid, it.key(), -1, fSaleDoc, cashamount, cashUUID, 1)) {
            return returnFalse(dw.fErrorMsg, &db);
        }
    }

    if(haveStore) {
        if(dw.writeInput(storeDocId, err)) {
            if(!dw.writeAHeader(storeDocId, storedocUserNum, DOC_STATE_SAVED, DOC_TYPE_STORE_INPUT, uid, QDate::currentDate(),
                                QDate::currentDate(), QTime::currentTime(), 0, 0, storeDocComment,
                                0, __c5config.getValue(param_default_currency).toInt())) {
                return returnFalse(dw.fErrorMsg, &db);
            }

            dw.writeTotalStoreAmount(storeDocId);
        }
    }

    if(!C5Config::localReceiptPrinter().isEmpty()) {
        PrintReceipt p;
        p.print(oheader._id(), db);
    }

    close();
    C5Message::info(tr("Return completed"));
}

void ViewOrder::on_tbl_cellClicked(int row, int column)
{
    if(column < 2) {
        return;
    }

    if(!property("return").toBool()) {
        return;
    }

    double maxqty = ui->tbl->item(row, 3)->data(Qt::UserRole).toDouble();
    double newqty = DQty::getQty(tr("Quantity"), maxqty, this);

    if(newqty < 0) {
        return;
    }

    ui->tbl->setDouble(row, 3, newqty);
    countOrder();
}

void ViewOrder::on_btnEditReason_clicked()
{
}

void ViewOrder::on_btnPrintPrices_clicked()
{
    QString ppp = "f_price1";

    if(C5Message::question("Զեղչված՞") == QDialog::Accepted) {
        ppp = "f_price1disc";
    }

    QPrintDialog pd;
    QString printerName;

    if(pd.exec() == QDialog::Accepted) {
        printerName = pd.printer()->printerName();
    } else {
        return;
    }

    C5Database db;
    db[":f_header"] = ui->leUUID->text();
    QString sql = R"(
    SELECT og.f_goods, gc.f_goods as gg,
    cast(gc.f_qty as unsigned) as f_qty,
    cast(og.f_qty as  unsigned) as f_qty2,
    coalesce(gp.%1,0) as fp1,
    coalesce(gp2.%1,0) as fp2
    FROM o_goods og
    LEFT JOIN c_goods g ON g.f_id=og.f_goods
    LEFT JOIN c_goods_complectation gc ON gc.f_base=og.f_goods and g.f_unit=10
    LEFT JOIN c_goods_prices gp ON gp.f_goods=gc.f_goods
    LEFT JOIN c_goods_prices gp2 ON gp2.f_goods=og.f_goods
    WHERE f_header=:f_header
    ORDER BY og.f_row desc
              )";
    sql.replace("%1", ppp);
    db.exec(sql);

    while(db.nextRow()) {
        QFont font(qApp->font());
        font.setPointSize(44);
        font.setBold(true);
        C5Printing p;

        if(ppp == "f_price1") {
            p.setSceneParams(350, 100, QPageLayout::Portrait);
        } else {
            p.setSceneParams(350, 100, QPageLayout::Portrait);
        }

        p.setFont(font);
        p.br(2);
        p.br(6);
        bool print = false;
        int qty =  db.getInt("f_qty");
        //p.line(4);
        p.line(50, p.fTop, 270, p.fTop, 4);

        if(db.getInt("gg") == 0) {
            if(db.getDouble("fp2") > 0) {
                print = true;
                p.ctext(float_str(db.getDouble("fp2"), 1) + "֏");
                qty =  db.getInt("f_qty2");
            }
        } else {
            if(db.getDouble("fp1") > 0) {
                print = true;
                p.ctext(float_str(db.getDouble("fp1"), 1) + "֏");
            }
        }

        if(print) {
            for(int i = 0; i < qty; i++) {
                p.print(printerName, QPageSize::Custom);
            }
        }
    }
}
