#include "worder.h"
#include "outputofheader.h"
#include "ui_worder.h"
#include "c5database.h"
#include "c5config.h"
#include "printtaxn.h"
#include "dqty.h"
#include "c5message.h"
#include "c5permissions.h"
#include "printreceiptgroup.h"
#include "selectstaff.h"
#include "c5shoporder.h"
#include "c5replacecharacter.h"
#include "struct_partner.h"
#include "c5structtableview.h"
#include "working.h"
#include "selectprinters.h"
#include "dlgpaymentchoose.h"
#include "c5user.h"
#include "datadriver.h"
#include "wcustomerdisplay.h"
#include "c5utils.h"
#include "c5checkbox.h"
#include "goodscols.h"
#include <QInputDialog>
#include <QSettings>
#include <QDir>
#include <QFile>

static QSettings s(_ORGANIZATION_, _APPLICATION_ + QString("\\") + _MODULE_);

WOrder::WOrder(C5User *user, int saleType, WCustomerDisplay *customerDisplay, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WOrder)
{
    ui->setupUi(this);
    ui->leTotal->setText("0");
    fDraftSale.saleType = saleType;
    fDraftSale.state = 1;
    fDraftSale.date = QDate::currentDate();
    fDraftSale.time = QTime::currentTime();
    fDraftSale.staff = user->id();
    fDraftSale.hall = __c5config.defaultHall();
    fOHeader.id = C5Database::uuid();
    fOHeader.state = ORDER_STATE_CLOSE;
    fOHeader.hall = __c5config.defaultHall();
    fOHeader.table = __c5config.defaultTable();
    fOHeader.cashier = user->id();
    fOHeader.staff = user->id(),
    fOHeader.comment = "";
    fOHeader.source = 2;
    fOHeader.print = 1;
    fOHeader.saleType = saleType;
    fOHeader.currency = __c5config.getValue(param_default_currency).toInt();
    fCustomerDisplay = customerDisplay;
    fUser = user;
    fGiftCard = 0;
    fWorking = static_cast<Working*>(parent);
    ui->lbDisc->setVisible(false);
    ui->leDisc->setVisible(false);
    ui->lbUseAccumulated->setVisible(false);
    ui->leUseAccumulated->setVisible(false);
    ui->leCurrentAccumulated->setVisible(false);
    ui->lbCurrentAccumlated->setVisible(false);
    noImage();
    ui->tblData->setColumnWidth(col_action, 40);
    ui->tblData->setColumnWidth(col_bacode, 150);
    ui->tblData->setColumnWidth(col_group, 150);
    ui->tblData->setColumnWidth(col_name, 400);
    ui->tblData->setColumnWidth(col_qty, 100);
    ui->tblData->setColumnWidth(col_unit, 100);
    ui->tblData->setColumnWidth(col_price, 100);
    ui->tblData->setColumnWidth(col_total, 100);
    ui->tblData->setColumnWidth(col_discamount, 100);
    ui->tblData->setColumnWidth(col_discmode, 30);
    ui->tblData->setColumnWidth(col_discvalue, 120);
    ui->tblData->setColumnWidth(col_stock, 100);
    ui->tblData->setColumnWidth(col_qr, 50);
    ui->tblData->setColumnWidth(col_check_discount, 50);
    ui->tblData->setColumnHidden(col_group, !s.value("col" + QString::number(col_group)).toBool());
    ui->tblData->setColumnHidden(col_unit, !s.value("col" + QString::number(col_unit)).toBool());
    ui->tblData->setColumnHidden(col_qtybox, !s.value("col" + QString::number(col_qtybox)).toBool());
    ui->tblData->setColumnHidden(col_discamount, !s.value("col" + QString::number(col_discmode)).toBool());
    ui->tblData->setColumnHidden(col_discmode, !s.value("col" + QString::number(col_discmode)).toBool());
    ui->tblData->setColumnHidden(col_discvalue, !s.value("col" + QString::number(col_discvalue)).toBool());
    ui->tblData->setColumnHidden(col_qr, !s.value("col" + QString::number(col_qr)).toBool());
    ui->tblData->setColumnHidden(col_check_discount, !s.value("col" + QString::number(col_check_discount)).toBool());
    fHttp = new NInterface(this);
    ui->btnF1->setVisible(false);
    ui->btnF2->setVisible(false);
    ui->btnF3->setVisible(fWorking->flag(3).enabled || true); //TODO
    ui->btnF4->setVisible(true);
    ui->btnF5->setVisible(false);
    // ui->btnF1->setText(fWorking->flag(1).name);
    // ui->btnF2->setText(fWorking->flag(2).name);
    // ui->btnF3->setText(fWorking->flag(3).name);
    // ui->btnF4->setText(fWorking->flag(4).name);
    // ui->btnF5->setText(fWorking->flag(5).name);
    C5Database db;
    QString err;
    fDraftSale.write(db, err);
}

WOrder::~WOrder()
{
    removeDraft();
    delete ui;
}

void WOrder::setPrepaidAmount(double amountPrepaid)
{
    fOHeader.amountPrepaid = amountPrepaid;
}

void WOrder::updateCustomerDisplay(WCustomerDisplay *cd)
{
    fCustomerDisplay = cd;

    if(fCustomerDisplay) {
        fCustomerDisplay->clear();

        for(int i = 0; i < fOGoods.count(); i++) {
            const OGoods &g = fOGoods.at(i);
            fCustomerDisplay->addRow(g._goodsName, float_str(g.qty, 2), float_str(g.price, 2), float_str(g.total, 2),
                                     float_str(g.discountAmount, 2));
        }

        fCustomerDisplay->setTotal(ui->leTotal->text());
    }
}

void WOrder::clearCode()
{
    ui->leCode->clear();
    ui->leCode->setFocus();
}

void WOrder::keyMinus()
{
    if(ui->leCode->text().length() > 1) {
        return;
    }

    clearCode();
    removeRow();
}

void WOrder::keyPlus()
{
    if(ui->leCode->text().length() > 1) {
        return;
    }

    clearCode();

    if(__c5config.getValue(param_shop_deny_qtychange).toInt() == 0) {
        changeQty();
    }
}

void WOrder::keyAsterix()
{
    if(ui->leCode->text().length() > 1) {
        return;
    }

    clearCode();
    changePrice();
}

bool WOrder::writeOrder()
{
    if(ui->btnF4->isChecked()) {
        if(ui->leTIN->text().length() != 8) {
            C5Message::error(tr("Invalid taxpayer tin"));
            return false;
        }
    }

    if(fOGoods.count() == 0) {
        C5Message::error(tr("Empty order"));
        return false;
    }

    if(__c5config.cashId() == 0) {
        C5Message::error(tr("Cashdesk for cash not defined"));
        return false;
    }

    if(__c5config.nocashId() == 0) {
        C5Message::error(tr("Cashdesk for card not defined"));
        return false;
    }

    bool prepaidReadonly = false;

    if(ui->leUseAccumulated->getDouble() > 0) {
        fOHeader.amountPrepaid = ui->leUseAccumulated->getDouble();
        prepaidReadonly = true;
    }

    if(fOHeader.amountCash < 0.001
            && fOHeader.amountCard < 0.001
            && fOHeader.amountBank < 0.001
            && fOHeader.amountCredit < 0.001
            && fOHeader.amountIdram < 0.001
            && fOHeader.amountTelcell < 0.001
            && fOHeader.amountPayX < 0.001
            && fOHeader.amountPrepaid < 0.001) {
        fOHeader.amountCash = fOHeader.amountTotal;
    }

    fOHeader._printFiscal = __c5config.alwaysOfferTaxPrint();

    if(ui->btnF5->isChecked()) {
        fOHeader.amountCash = 0;
        fOHeader.amountCard = 0;
        fOHeader.amountIdram = 0;
        fOHeader.amountTelcell = 0;
        fOHeader.amountBank = 0;
        fOHeader.amountCredit = 0;
        fOHeader.amountPrepaid = 0;
        fOHeader.amountHotel = 0;
        fOHeader.amountDebt = fOHeader.amountTotal;
    }

    //TODO replace prepaid max amount
    if(!DlgPaymentChoose::getValues(fUser, fOHeader.amountTotal, fOHeader.amountCash, fOHeader.amountCard, fOHeader.amountIdram,
                                    fOHeader.amountTelcell, fOHeader.amountBank, fOHeader.amountCredit,
                                    fOHeader.amountPrepaid, fOHeader.amountDebt,
                                    fOHeader.amountCashIn, fOHeader.amountChange, fOHeader._printFiscal, prepaidReadonly,
                                    0)) {
        return false;
    }

    QElapsedTimer t;
    t.start();
    C5Database db;
    QString sql;

    if((fOHeader.amountDebt > 0.001 || fOHeader.amountBank > 0.001) && fOHeader.partner == 0) {
        C5Message::error(tr("Debt impossible on unknown partner"));
        return false;
    }

    for(int i = 0; i < fOGoods.count(); i++) {
        const OGoods &g = fOGoods.at(i);

        if(g.qty < 0.0001) {
            C5Message::error(tr("Invalid qty"));
            return false;
        }

        if(g.price < 0) {
            C5Message::error(tr("Invalid price"));
            return false;
        }
    }

    SelectStaff ss(fWorking, fUser);
    int worker = 0;

    if(__c5config.shopDifferentStaff() && fWorking->fCurrentUsers.count() > 0) {
        if(ss.exec() == QDialog::Rejected) {
            return false;
        }

        worker = ss.mUserId;
    }

    QJsonObject jdoc;
    jdoc["session"] = C5Database::uuid();
    jdoc["giftcard"] = fGiftCard;
    jdoc["settings"] = __c5config.fSettingsName;
    //TODO jdoc["organization"] = ui->leOrganization->text();
    //TODO jdoc["contact"] = ui->leContact->text();
    QJsonObject jh;
    jh["f_id"] = fOHeader._id().isEmpty() ? fDraftSale.id.toString() : fOHeader._id();
    jh["f_hallid"] = fOHeader.hallId;
    jh["f_prefix"] = fOHeader.prefix;
    jh["f_state"] = ORDER_STATE_CLOSE;
    jh["f_hall"] = fOHeader.hall;
    jh["f_table"] = fOHeader.table;
    jh["f_dateopen"] = fOHeader.dateOpen.toString(FORMAT_DATE_TO_STR_MYSQL);
    jh["f_dateclose"] = QDate::currentDate().toString(FORMAT_DATE_TO_STR_MYSQL);
    jh["f_datecash"] = QDate::currentDate().toString(FORMAT_DATE_TO_STR_MYSQL);
    jh["f_timeopen"] = fOHeader.timeOpen.toString(FORMAT_TIME_TO_STR);
    jh["f_timeclose"] = QTime::currentTime().toString(FORMAT_TIME_TO_STR);
    jh["f_cashier"] = fOHeader.cashier;
    jh["f_staff"] = worker;
    jh["f_comment"] = ui->leComment->text();
    jh["f_print"] = fOHeader.print;
    jh["f_amounttotal"] = fOHeader.amountTotal;
    jh["f_amountcash"] = fOHeader.amountCash;
    jh["f_amountcard"] = fOHeader.amountCard;
    jh["f_amountprepaid"] = fOHeader.amountPrepaid;
    jh["f_amountbank"] = fOHeader.amountBank;
    jh["f_amountcredit"] = fOHeader.amountCredit;
    jh["f_amountidram"] = fOHeader.amountIdram;
    jh["f_amounttelcell"] = fOHeader.amountTelcell;
    jh["f_amountdebt"] = fOHeader.amountDebt;
    jh["f_amountpayx"] = fOHeader.amountPayX;
    jh["f_amountother"] = fOHeader.amountOther;
    jh["f_amountservice"] = fOHeader.amountService;
    jh["f_amountdiscount"] = fOHeader.amountDiscount;
    jh["f_servicefactor"] = fOHeader.serviceFactor;
    jh["f_discountfactor"] = fOHeader.discountFactor;
    jh["f_source"] = fOHeader.source;
    jh["f_saletype"] = fOHeader.saleType;
    jh["f_partner"] = fOHeader.partner;
    jh["f_currency"] = fOHeader.currency;
    jh["f_taxpayertin"] = fOHeader.taxpayerTin;
    jh["f_cash"] = fOHeader.amountCashIn;
    jh["f_change"] = fOHeader.amountChange;
    jdoc["header"] = jh;
    QJsonArray jg;

    for(int i = 0; i < fOGoods.count(); i++) {
        OGoods &g = fOGoods[i];
        QJsonObject jt;
        jt["f_id"] = C5Database::uuid();
        jt["f_header"] = jh["f_id"];
        jt["f_store"] = g.store;
        jt["f_goods"] = g.goods;
        jt["f_qty"] = g.qty;
        jt["f_price"] = g.price;
        jt["f_total"] = g.total;
        jt["f_tax"] = g.tax;
        jt["f_sign"] = g.sign;
        jt["f_taxdebt"] = g.taxDept;
        jt["f_adgcode"] = g.adgCode;
        jt["f_row"] = i;
        jt["f_storerec"] = g.storeRec;
        jt["f_discountfactor"] = g.discountFactor;
        jt["f_discountmode"] = g.discountMode;
        jt["f_discountamount"] = g.discountAmount;
        jt["f_return"] = g.return_;
        jt["f_returnfrom"] = g.returnFrom.isEmpty() ? QJsonValue() : g.returnFrom;
        jt["f_isservice"] = g.isService;
        jt["f_amountaccumulate"] = g.accumulateAmount;
        jt["f_emarks"] = QString(g.emarks).replace("\"", "\\\"");
        jg.append(jt);
    }

    jdoc["goods"] = jg;
    QJsonObject jhistory;
    jhistory["f_card"] = fBHistory.card;
    jhistory["f_data"] = fBHistory.data;
    jhistory["f_type"] = fBHistory.type;
    jhistory["f_value"] = fBHistory.value;

    if(fBHistory.card > 0 && fBHistory.type == CARD_TYPE_ACCUMULATIVE) {
        QJsonObject jtemp;
        jtemp["card"] = fBHistory.card;
        jtemp["accumulate"] = ui->leCurrentAccumulated->getDouble();
        jtemp["accumulatespend"] = ui->leUseAccumulated->getDouble();
        jdoc["accumulate"] = jtemp;
        jhistory["f_data"] = ui->leCurrentAccumulated->getDouble() > 0 ? ui->leCurrentAccumulated->getDouble() : -1 *  ui->leUseAccumulated->getDouble();
    }

    jdoc["history"] = jhistory;
    QJsonObject jflags;
    jflags["f_1"] = ui->btnF1->isChecked() ? 1 : 0;
    jflags["f_2"] = ui->btnF2->isChecked() ? 1 : 0;
    jflags["f_3"] = ui->btnF3->isChecked() ? 1 : 0;
    jflags["f_4"] = ui->btnF4->isChecked() ? 1 : 0;
    jflags["f_5"] = ui->btnF5->isChecked() ? 1 : 0;
    jdoc["flags"] = jflags;
    sql = QString(QJsonDocument(jdoc).toJson(QJsonDocument::Compact));
    sql.replace("'", "\\'");
    sql = QString("call sf_create_shop_order('%1')").arg(sql);

    if(!db.exec(sql)) {
        C5Message::error(db.fLastError);
        return false;
    }

    db[":f_session"] = jdoc["session"].toString();

    if(!db.exec("select f_result from a_result where f_session=:f_session")) {
        C5Message::error(db.fLastError);
        return false;
    }

    if(!db.nextRow()) {
        C5Message::error("Program error. Cannot get result of session");
        return false;
    }

    QJsonObject jr = QJsonDocument::fromJson(db.getString("f_result").toUtf8()).object();

    if(jr["status"].toInt() != 1) {
        C5Message::error(jr["message"].toString());
        return false;
    }

    if(C5Config::taxIP().isEmpty()) {
        fOHeader._printFiscal = false;
    }

    /* PRINT FISCAl */

    if(fOHeader._printFiscal) {
        QElapsedTimer et;
        et.restart();
        PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(),
                     fOHeader.hasIdram() ? "true" : C5Config::taxUseExtPos(), C5Config::taxCashier(), C5Config::taxPin(), this);
        QString jsonIn, jsonOut, err;
        int result = 0;

        if(ui->btnF4->isChecked()) {
            pt.fPartnerTin = ui->leTIN->text();
        }

        if(fOHeader.saleType != -1) {
            for(int i = 0; i < fOGoods.count(); i++) {
                OGoods &g = fOGoods[i];

                if(g.emarks.isEmpty() == false) {
                    pt.fEmarks.append(g.emarks);
                }

                if(g.discountFactor > 0.999) {
                    continue;
                }

                pt.addGoods(g.taxDept, //dep
                            g.adgCode, //adg
                            QString::number(g.goods), //goods id
                            g._goodsFiscalName.isEmpty() ? g._goodsName : g._goodsFiscalName, //name
                            g.price, //price
                            g.qty, //qty
                            g.discountFactor * 100); //discount
            }

            do {
                if(__c5config.getValue(param_simple_fiscal).toInt() == 1) {
                    if(__c5config.fMainJson["tax_dept"].toString().toInt() == 0) {
                        C5Message::error(tr("Tax department is not set") + "<br>" + __c5config.fMainJson["tax_dept"].toString());
                        return false;
                    }

                    result = pt.makeJsonAndPrintSimple(__c5config.fMainJson["tax_dept"].toString().toInt(), fOHeader.amountCard, 0, jsonIn, jsonOut, err);
                } else {
                    result = pt.makeJsonAndPrint(fOHeader.amountCard + fOHeader.amountIdram + fOHeader.amountTelcell,
                                                 fOHeader.amountPrepaid, jsonIn, jsonOut, err);
                }

                if(result != pt_err_ok) {
                    if(err.contains("-5")) {
                        err = tr("Connection with fiscal machine lost");
                    }

                    QJsonParseError jsonErr;
                    QJsonObject jtax;
                    jtax["f_order"] = fOHeader._id();
                    jtax["f_elapsed"] = et.elapsed();
                    jtax["f_in"] = QJsonDocument::fromJson(jsonIn.replace("'", "''").toUtf8(), &jsonErr).object();
                    jtax["f_out"] = jsonOut;
                    jtax["f_err"] = err;
                    jtax["f_result"] = result;
                    jtax["f_state"] = result == pt_err_ok ? 1 : 0;
                    sql = QString(QJsonDocument(jtax).toJson(QJsonDocument::Compact));
                    sql.replace("\\\"", "\\\\\"");
                    sql.replace("'", "\\'");
                    db.exec(QString("call sf_create_shop_tax('%1')").arg(sql));

                    switch(C5Message::question(err, tr("Try again"), tr("Do not print fiscal"))) {
                    case QDialog::Rejected:
                        result = pt_err_ok;
                        break;

                    case QDialog::Accepted:
                        break;
                    }
                }
            } while(result != pt_err_ok);
        } else {
            if(__c5config.getValue(param_simple_fiscal).toInt() == 1) {
                result = pt.makeJsonAndPrintSimple(__c5config.fMainJson["tax_dept"].toString().toInt(), fOHeader.amountCard, 0, jsonIn, jsonOut, err);
            } else {
                result = pt.printAdvanceJson(fOHeader.amountCash, fOHeader.amountCard, jsonIn, jsonOut, err);
            }
        }

        QJsonParseError jerr;
        QJsonDocument jod = QJsonDocument::fromJson(jsonOut.toUtf8(), &jerr);

        if(jerr.error != QJsonParseError::NoError) {
            err = jerr.errorString();
            jod = QJsonDocument::fromJson(QString("{\"data\":\"" + jsonOut + "\"").toUtf8(), &jerr);
        }

        QDir dir = QDir::tempPath();
        QFile file(dir.tempPath() + "/printtax_errors.txt");

        if(file.open(QIODevice::Append)) {
            file.write(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss").toUtf8());
            file.write("\r\n");
            file.write("IN");
            file.write("\r\n");
            file.write(jsonIn.toUtf8());
            file.write("\r\n");
            file.write("OUT");
            file.write("\r\n");
            file.write(jsonOut.toUtf8());
            file.write("\r\n");
            file.write("\r\n ERROR JSON");
            file.write("\r\n");
            file.write(err.toUtf8());
            jerr.errorString();
            file.write("\r\n");
            file.close();
        }

        QJsonParseError jsonErr;
        QJsonObject jtax;
        jtax["f_order"] = fOHeader._id();
        jtax["f_elapsed"] = et.elapsed();
        jtax["f_in"] = QJsonDocument::fromJson(jsonIn.replace("'", "''").toUtf8(), &jsonErr).object();
        jtax["f_out"] = jod.object();
        jtax["f_err"] = err;
        jtax["f_result"] = result;
        jtax["f_state"] = result == pt_err_ok ? 1 : 0;
        sql = QString(QJsonDocument(jtax).toJson(QJsonDocument::Compact));
        sql.replace("\\\"", "\\\\\"");
        sql.replace("'", "\\'");
        db.exec(QString("call sf_create_shop_tax('%1')").arg(sql));

        if(result != pt_err_ok) {
            C5Message::error(err);
        }
    }

    /* end fiscal */

    if(fOHeader.partner > 0) {
        OutputOfHeader ooh;
        ooh.make(db, fOHeader._id());
    }

    if(!fDraftSale.id.toString().isEmpty()) {
        db[":f_id"] = fDraftSale._id();
        db.exec("update o_draft_sale set f_state=3 where f_id=:f_id");
        db[":f_header"] = fDraftSale.id;
        db.exec("update o_draft_sale_body set f_state=3 where f_header=:f_header");
    }

    qDebug() << "Order writed in" << t.elapsed();

    if(!C5Config::localReceiptPrinter().isEmpty()) {
        PrintReceiptGroup p;

        switch(C5Config::shopPrintVersion()) {
        case 1: {
            bool p1, p2;

            if(SelectPrinters::selectPrinters(p1, p2, fUser)) {
                if(p1) {
                    p.print(fOHeader._id(), db, 1);
                }

                if(p2) {
                    p.print(fOHeader._id(), db, 2);
                }
            }

            break;
        }

        case 2:
            p.print2(fOHeader._id(), db);
            break;

        case 3:
            p.print3(fOHeader._id(), db);
            break;

        default:
            break;
        }
    }

    return true;
}

void WOrder::fixCostumer(const QString &code)
{
    C5Database db;
    db[":f_code"] = code;
    db.exec("select * from b_cards_discount where f_code=:f_code");

    if(!db.nextRow()) {
        return;
    }

    if(QDate::currentDate() > db.getDate("f_dateend")) {
        C5Message::error(tr("Cards was expired"));
        return;
    }

    fOHeader.partner = db.getInt("f_client");
    fBHistory.card = db.getInt("f_id");
    fBHistory.type = db.getInt("f_mode");
    fBHistory.value = db.getDouble("f_value");
    db[":f_id"] = fOHeader.partner;
    db.exec("select * from c_partners where f_id=:f_id");

    if(!db.nextRow()) {
        return;
    }

    if(__c5config.getValue(param_auto_discount) != code) {
        //TODO
        // if(!checkDiscountRight()) {
        //     return;
        // }
    }

    if(fBHistory.value < 0) {
        double v;

        if(!getDiscountValue(fBHistory.type, v)) {
            fBHistory.card = 0;
            fBHistory.value = 0;
            fBHistory.data = 0;
            return;
        }

        fBHistory.value = v;

        if(fBHistory.type == CARD_TYPE_DISCOUNT) {
            fBHistory.value = v / 100;
        } else {
            fBHistory.value = v;
        }
    }

    for(int i = 0; i < fOGoods.count(); i++) {
        OGoods &g = fOGoods[i];
        g.discountMode = fBHistory.type;
        g.discountFactor = fBHistory.value;
    }

    countTotal();
}

void WOrder::changeQty()
{
    int row = ui->tblData->currentRow();

    if(row < 0) {
        return;
    }

    if(fOGoods.at(row).discountMode == CARD_TYPE_MANUAL) {
        C5Message::error(tr("Cannot change the quantity on selected row with manual discount mode"));
        return;
    }

    if(fOGoods.at(row).emarks.isEmpty() == false) {
        return;
    }

    double qty = DQty::getQty(tr("Quantity"), 0, this);

    if(qty < 0.001) {
        return;
    }

    setQtyOfRow(row, qty);
}

void WOrder::changeQty(double qty)
{
    int row = ui->tblData->currentRow();

    if(row < 0) {
        return;
    }

    if(fOGoods.at(row).discountMode == CARD_TYPE_MANUAL) {
        C5Message::error(tr("Cannot change the quantity on selected row with manual discount mode"));
        return;
    }

    if(qty < 0.001) {
        return;
    }

    setQtyOfRow(row, qty);
}

void WOrder::discountRow(const QString &code)
{
    int row = ui->tblData->currentRow();

    if(row < 0) {
        return;
    }

    OGoods &g = fOGoods[row];

    if(g.discountFactor > 0.001) {
        C5Message::error(tr("Discount already applied"));
        return;
    }

    C5Database db;
    db[":f_code"] = code;
    db.exec("select * from b_cards_discount where f_code=:f_code");

    if(!db.nextRow()) {
        return;
    }

    if(QDate::currentDate() > db.getDate("f_dateend")) {
        C5Message::error(tr("Cards was expired"));
        return;
    }

    double v = db.getDouble("f_value") / 100;
    int discType = db.getInt("f_mode");

    if(db.getDouble("f_value") < 0) {
        if(!getDiscountValue(discType, v)) {
            return;
        }

        if(discType == CARD_TYPE_DISCOUNT) {
            v /= 100;
        }
    }

    g.discountMode = discType;

    switch(discType) {
    case CARD_TYPE_DISCOUNT:
        g.discountFactor = v;
        g.discountMode = discType;
        break;

    case CARD_TYPE_MANUAL:
        if(v > g.total) {
            C5Message::error(tr("Discount amount greater than total amount"));
            return;
        }

        g.discountFactor = v;
        g.discountAmount = v;
        break;

    default:
        C5Message::error(tr("This discount mode is not supported"));
        return;
    }

    countTotal();
}

void WOrder::changePrice()
{
    int row = ui->tblData->currentRow();

    if(row < 0) {
        return;
    }

    OGoods &g = fOGoods[row];
    double currentPrice = g.price;

    if(__c5config.shopDenyPriceChange() && !(currentPrice < 0)) {
        return;
    }

    double price = DQty::getQty(tr("Price"), 0, this);

    if(price < 0.001) {
        return;
    }

    g.price = price;
    C5Database db;
    db[":f_id"] = ui->tblData->item(row, 0)->data(Qt::UserRole + 101);
    db[":f_price"] = g.price;
    db.exec("update o_draft_sale_body set f_price=:f_price where f_id=:f_id");
    countTotal();
}

void WOrder::changePrice(double price)
{
    int row = ui->tblData->currentRow();

    if(row < 0) {
        return;
    }

    fOGoods[row].price = price;
    C5Database db;
    db[":f_id"] = ui->tblData->item(row, 0)->data(Qt::UserRole + 101);
    db[":f_price"] = price;
    db.exec("update o_draft_sale_body set f_price=:f_price where f_id=:f_id");
    countTotal();
}

int WOrder::rowCount()
{
    return fOGoods.count();
}

void WOrder::removeRow()
{
    int row = ui->tblData->currentRow();

    if(row < 0) {
        return;
    }

    C5User *tmp = fUser;

    if(!tmp->check(cp_t5_remove_row_from_shop)) {
        QString password = QInputDialog::getText(this, tr("Password"), tr("Password"), QLineEdit::Password);
        C5User *tmp = new C5User;

        if(tmp->error().isEmpty()) {
        } else {
            C5Message::error(tmp->error());
            delete tmp;
            return;
        }
    }

    OGoods &g = fOGoods[row];
    QString name = g._goodsName;
    C5Database db;
    db[":f_id"] = ui->tblData->item(row, 0)->data(Qt::UserRole + 101);
    db[":f_state"] = 2;
    db.exec("update o_draft_sale_body set f_state=:f_state where f_id=:f_id");
    fOGoods.remove(row);
    ui->tblData->setRowCount(fOGoods.count());
    countTotal();
}

void WOrder::nextRow()
{
    int row = ui->tblData->currentRow();

    if(row + 1 < ui->tblData->rowCount()) {
        ui->tblData->setCurrentItem(ui->tblData->item(row + 1, 0));
    }
}

void WOrder::prevRow()
{
    int row = ui->tblData->currentRow();

    if(row - 1 > -1) {
        ui->tblData->setCurrentItem(ui->tblData->item(row - 1, 0));
    }
}

int WOrder::lastRow()
{
    return ui->tblData->rowCount() - 1;
}

void WOrder::comma()
{
    clearCode();

    if(__c5config.getValue(param_shop_deny_qtychange).toInt() == 0) {
        int row = ui->tblData->currentRow();

        if(row < 0) {
            return;
        }

        if(fOGoods.at(row).discountMode == CARD_TYPE_MANUAL) {
            C5Message::error(tr("Cannot change the quantity on selected row with manual discount mode"));
            return;
        }

        double qty = DQty::getQty(tr("Box"), 0, this);

        if(qty < 0.001) {
            return;
        }

        setQtyOfRow(row, qty);
    }
}

void WOrder::setDiscount(const QString &label, const QString &value)
{
    if(label.isEmpty()) {
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
    double accAmount = 0;
    fOHeader.countAmount(fOGoods, fBHistory);
    ui->leTotal->setDouble(fOHeader.amountTotal);

    if(fBHistory.type == CARD_TYPE_DISCOUNT) {
        ui->leDisc->setDouble(fOHeader.amountDiscount);
    }

    for(int i = 0; i < fOGoods.count(); i++) {
        OGoods &og = fOGoods[i];
        ui->tblData->setData(i, col_bacode, og._barcode);
        ui->tblData->setData(i, col_group, og._groupName);
        ui->tblData->setData(i, col_name, og._goodsName);
        ui->tblData->setData(i, col_qty, og.qty);
        ui->tblData->setData(i, col_qtybox, og.qty);
        ui->tblData->setData(i, col_price, og.price);
        ui->tblData->setData(i, col_unit, og._unitName);
        ui->tblData->setData(i, col_total, og.total);
        ui->tblData->setData(i, col_discamount, og.discountAmount);
        ui->tblData->setData(i, col_discvalue, og.discountFactor);
        ui->tblData->setData(i, col_discmode, og.discountMode);
        QPushButton *btn = static_cast<QPushButton*>(ui->tblData->cellWidget(i, col_qr));

        if(btn == nullptr) {
            btn = new QPushButton();
            btn->setFocusPolicy(Qt::NoFocus);
        }

        if(og.emarks.isEmpty()) {
            btn->setIcon(QPixmap(""));
        } else {
            btn->setIcon(QPixmap(":/qr_code.png").scaled(30, 30));
        }

        connect(btn, &QPushButton::clicked, this, &WOrder::readEmarks);
        ui->tblData->setCellWidget(i, col_qr, btn);

        if(fBHistory.card > 0 && fBHistory.type == CARD_TYPE_ACCUMULATIVE) {
            auto *ch = static_cast<C5CheckBox*>(ui->tblData->cellWidget(i, col_check_discount));
            ch->setFocusPolicy(Qt::NoFocus);

            if(ch->isChecked()) {
                og.accumulateAmount = og.total * (fBHistory.value / 100);
                accAmount += og.accumulateAmount;
            }
        }
    }

    ui->leCurrentAccumulated->setDouble(accAmount);
    ui->leTotal->setDouble(fOHeader.amountTotal);
    C5Database db;
    QString err;
    fDraftSale.amount = fOHeader.amountTotal;
    fDraftSale.write(db, err);
    updateCustomerDisplay(fCustomerDisplay);
}

bool WOrder::returnFalse(const QString &msg, C5Database &db)
{
    Q_UNUSED(db);
    C5Message::error(msg);
    return false;
}

bool WOrder::getDiscountValue(int discountType, double& v)
{
    bool ok = true;
    QString disctitle;
    double maxvalue = 100;

    switch(discountType) {
    case CARD_TYPE_DISCOUNT:
        disctitle = tr("Enter discount value");
        break;

    case CARD_TYPE_MANUAL:
        disctitle = tr("Enter discount amount");
        maxvalue = 999999999;
        break;

    default:
        return false;
    }

    v = QInputDialog::getDouble(this, tr("Discount"), disctitle, 0, 0.001, maxvalue, 3, &ok);
    return ok;
}

void WOrder::removeDraft()
{
    if(!fDraftSale._id().isEmpty() && ui->tblData->rowCount() == 0) {
        C5Database db;
        db[":f_id"] = fDraftSale._id();
        db.exec("update o_draft_sale set f_state=3 where f_id=:f_id");
        db[":f_header"] = fDraftSale._id();
        db.exec("update o_draft_sale_body set f_state=3 where f_header=:f_header");
    }
}

bool WOrder::setQtyOfRow(int row, double qty)
{
    OGoods &og = fOGoods[row];
    DbGoods g(og.goods);

    if(g.acceptIntegerQty()) {
        qty = trunc(qty);
    }

    double diffqty = qty ;

    if(og.isService == 0  && __c5config.getValue(param_shop_dont_check_qty).toInt() == 0) {
        QString err;

        if(!checkQty(g.id(), diffqty, err, og.qty)) {
            C5Message::error(og._goodsName + " \r\n" + err);
            return false;
        }
    }

    og.qty = qty;
    C5Database db;
    db[":f_id"] = ui->tblData->item(row, 0)->data(Qt::UserRole + 101);
    db[":f_qty"] = og.qty ;
    db.exec("update o_draft_sale_body set f_qty=:f_qty where f_id=:f_id");
    countTotal();
    return true;
}

bool WOrder::setPriceOfRow(int row, double price)
{
    fOGoods[row].price = price;
    countTotal();
    return true;
}

C5ClearTableWidget* WOrder::table()
{
    return ui->tblData;
}

bool WOrder::checkQty(int goods, double qty, QString &err, double oldqty)
{
    DbGoods gd(goods);
    QJsonObject params;
    params["store"] = __c5config.defaultStore();
    params["goods"] = gd.storeGoods();
    params["qty"] = qty;
    params["oldqty"] = oldqty;
    C5Database db;
    db.exec(QString("select sf_check_goods_qty('%1')").arg(json_str(params)));

    if(db.nextRow()) {
        params = str_json(db.getString(0));

        if(params["status"].toInt() == 0) {
            err = params["data"].toString();
            return false;
        }
    }

    return true;
}

void WOrder::openDraft(const QString &draftid)
{
    fHttp->createHttpQuery("/engine/shop/open-draft.php", QJsonObject{{"id", draftid}}, SLOT(openDraftResponse(
                QJsonObject)));
}

void WOrder::addGoods2(const QString &barcode, double price)
{
    fHttp->createHttpQuery("/engine/shop/process-barcode.php",
    QJsonObject{{"code", barcode},
        {"store", __c5config.defaultStore()},
        {"draft_header", fDraftSale._id()},
        {"retail", fOHeader.saleType == SALE_RETAIL}},
    SLOT(reponseProcessCode(QJsonObject)), QJsonObject{{"price", price}});
}

void WOrder::openDraftResponse(const QJsonObject & jdoc)
{
    removeDraft();
    QJsonObject js = jdoc["ds"].toObject();
    fDraftSale.saleType = js["f_saletype"].toInt();
    fDraftSale.id = js["f_id"].toString();
    QJsonArray ja = jdoc["goods"].toArray();
    ui->tblData->setRowCount(0);
    fOGoods.clear();

    for(int i = 0; i < ja.size(); i++) {
        QJsonObject jo = ja.at(i).toObject();
        OGoods og;
        DbGoods g(jo["f_goods"].toInt());
        og._groupName = g.group()->groupName();
        og._goodsName = g.goodsName();
        og._goodsFiscalName = g.goodsFiscalName();
        og._unitName = g.unit()->unitName();
        og._barcode = g.scancode();
        og.header = fOHeader._id();
        og.goods = jo["f_goods"].toInt();
        og.taxDept = g.group()->taxDept();
        og.adgCode = g.group()->adgt();
        og.isService = g.isService();
        og.qty = jo["f_qty"].toDouble();
        og.price = fOHeader.saleType == SALE_RETAIL ? jo["f_price1"].toDouble() : jo["f_price2"].toDouble();
        og.store = __c5config.defaultStore();
        og.total = og.qty * og.price;
        og.discountFactor = fBHistory.value;
        og.discountMode = fBHistory.type;
        og.discountAmount = 0;
        og.canDiscount = g.canDiscount();
        fOGoods.append(og);
        int row = ui->tblData->addEmptyRow();
        auto *ch = new C5CheckBox();
        ch->setCheckable(s.value("learnaccumulate").toBool());
        ch->setChecked(jo["f_candiscount"].toInt() == 1);
        connect(ch, &C5CheckBox::clicked, this, &WOrder::checkCardClicked);
        ui->tblData->setCellWidget(row, col_check_discount, ch);
    }

    countTotal();
    fHttp->httpQueryFinished(sender());
}

void WOrder::readEmarks()
{
    int row = ui->tblData->currentRow();

    if(row < 0) {
        return;
    }

    bool ok = false;
    QString qr = QInputDialog::getText(this, tr("Emark"), tr("Emark"), QLineEdit::Normal, fOGoods[row].emarks, &ok);

    if(!ok) {
        return;
    }

    if(qr.length() > 0 && qr.length() < 20) {
        C5Message::error(tr("Incorrect eMarks"));
        ui->leCode->setFocus();
        return;
    }

    fOGoods[row].emarks = qr;
    countTotal();
}
void WOrder::noImage()
{
    //ui->wimage->setVisible(false);
}
void WOrder::checkCardClicked(bool v)
{
    if(s.value("learnaccumulate").toBool()) {
        int row = -1;

        for(int i = 0; i < ui->tblData->rowCount(); i++) {
            if(sender() == ui->tblData->cellWidget(i, col_check_discount)) {
                row = i;
                break;
            }
        }

        if(row > -1) {
            C5Database db;
            db[":f_candiscount"] = v ? 1 : 0;
            db.update("c_goods", "f_id", fOGoods[row].goods);
            dbgoods->updateField(fOGoods[row].goods, "f_candiscount", v ? 1 : 0);
        }
    }

    countTotal();
}
void WOrder::on_leCode_textChanged(const QString & arg1)
{
    if(arg1 == "+") {
        return;
    }

    if(arg1 == "-") {
        keyMinus();
        return;
    }

    if(arg1 == "*") {
        return;
    }

    if(arg1 == ".") {
        comma();
        return;
    }
}
void WOrder::on_leCode_returnPressed()
{
    QString code = C5ReplaceCharacter::replace(ui->leCode->text());

    if(code.isEmpty()) {
        return;
    }

    ui->leCode->clear();
    ui->leCode->setFocus();
    /* DISCOUNT ONE ROW */
    qDebug() << code.first(0);

    if(code.first(1).toLower() == "t") {
        code.removeFirst();

        if(ui->tblData->rowCount() == 0) {
            return;
        }

        if(ui->tblData->currentRow() < 0) {
            return;
        }

        fOGoods[ui->tblData->currentRow()].rowDiscount = true;
    }

    if(!fUser->fConfig["code_pattern_discount_card"].toString().isEmpty()) {
        if(code.startsWith(fUser->fConfig["code_pattern_discount_card"].toString())) {
            processCode(code, cp_t12_shop_discount, [this, code](const QString & c) {checkDiscountCardCode(c);});
            return;
        }
    }

    if(!fUser->fConfig["code_pattern_accumulate_card"].toString().isEmpty()) {
        if(code.startsWith(fUser->fConfig["code_pattern_accumulate_card"].toString())) {
            processAccumulateCard(code);
            return;
        }
    }

    if(!fUser->fConfig["code_pattern_present_card"].toString().isEmpty()) {
        if(code.startsWith(fUser->fConfig["code_pattern_present_card"].toString())) {
            processPresentCard(code);
            return;
        }
    }

    processCode(code, -1, [this, code](const QString & c) {checkGoodsCode(c);});
}

void WOrder::on_btnSearchPartner_clicked()
{
    QVector<PartnerItem> result = C5StructTableView::get<PartnerItem>(search_partner, false, false);

    if(result.isEmpty()) {
        return;
    }

    on_btnRemovePartner_clicked();
    PartnerItem pi = result.at(0);
    fOHeader.partner = pi.id;
    ui->leTIN->setText(pi.tin);
    ui->leCustomer->setText(pi.taxName + ", " + pi.contactName + ", " + pi.phone);
    ui->btnF5->setVisible(true);
}

void WOrder::on_leUseAccumulated_textChanged(const QString & arg1)
{
    //TODO
    // if(arg1.toDouble() > ui->leGiftCardAmount->getDouble()) {
    //     ui->leUseAccumulated->setDouble(ui->leGiftCardAmount->getDouble());
    // }
    if(ui->leUseAccumulated->getDouble() > ui->leTotal->getDouble()) {
        ui->leUseAccumulated->setDouble(ui->leTotal->getDouble());
    }
}

void WOrder::on_btnRemovePartner_clicked()
{
    fOHeader.partner  = 0;
    fOHeader.taxpayerTin.clear();
    ui->leTIN->clear();
    ui->leCustomer->clear();
    fBHistory.card = 0;
    fBHistory.type = 0;
    fBHistory.value = 0;

    for(int i = 0; i < fOGoods.count(); i++) {
        OGoods &og = fOGoods[i];
        og.discountAmount = 0;
        og.discountFactor = 0;
        og.discountMode = 0;
    }

    countTotal();
}

void WOrder::on_tblData_doubleClicked(const QModelIndex & index)
{
    qDebug() << index.column();

    switch(index.column()) {
    case col_discvalue: {
        if(fBHistory.card == 0) {
            return;
        }

        OGoods &g = fOGoods[index.row()];
        double discount = DQty::getQty(tr("Discount"), 100, this);

        if(discount < 0) {
            return;
        }

        g.discountFactor = discount;
        C5Database db;
        db[":f_id"] = ui->tblData->item(index.row(), 0)->data(Qt::UserRole + 101);
        db[":f_price"] = g.price;
        db.exec("update o_draft_sale_body set f_price=:f_price where f_id=:f_id");
        countTotal();
        break;
    }
    }
}

void WOrder::processCode(const QString & code, int permission, std::function<void (const QString&)> func)
{
    auto *user = new C5User(fUser);
    auto proceed = [user, code, permission, func]() {
        if(permission == -1 || user->check(permission)) {
            func(code);
        }

        user->deleteLater();
    };

    if(permission == -1 || user->check(permission)) {
        proceed();
        return;
    }

    bool ok = false;
    const QString password = QInputDialog::getText(
                                 this,
                                 tr("Password"),
                                 tr("Password"),
                                 QLineEdit::Password,
                                 QString(),
                                 &ok
                             );

    if(!ok || password.isEmpty()) {
        user->deleteLater();
        return;
    }

    user->authorize(password, fHttp,
    [proceed](const QJsonObject&) {
        proceed();
    },
    [user]() {
        user->deleteLater();
    });
}

void WOrder::checkDiscountCardCode(const QString &code)
{
    fHttp->createHttpQueryLambda("/engine/v2/shop/discount-system/get-info", {{"code", code}, {"need", CARD_TYPE_DISCOUNT}}, [this](const QJsonObject & jdoc) {
        QJsonObject card = jdoc["card"].toObject();
        QJsonObject history = jdoc["history"].toObject();
        QJsonObject partner = jdoc["partner"].toObject();
        fBHistory.card = card["f_id"].toInt();
        fBHistory.type = card["f_mode"].toInt();
        fBHistory.value = card["f_value"].toDouble();

        if(fBHistory.value < 0) {
            double v = fBHistory.value;

            if(!getDiscountValue(CARD_TYPE_DISCOUNT, v)) {
                fBHistory.card = 0;
                fBHistory.type = 0;
                fBHistory.value = 0;
                return;
            }

            fBHistory.value = v;
        }

        ui->leDisc->setText(QString("%1%").arg(float_str(card["f_value"].toDouble(), 2)));
        ui->leDisc->setVisible(true);
        ui->lbDisc->setVisible(true);
        ui->lbDisc->setText(QString("%1: %2%").arg(tr("Discount"), float_str(card["f_value"].toDouble(), 2)));
        ui->leCustomer->setText(partner["f_name"].toString());
        ui->leTIN->setText(partner["f_taxcode"].toString());
        fOHeader.partner = partner["f_id"].toInt();
        bool discountRow = false;

        for(int i = 0; i < fOGoods.count(); i++) {
            OGoods &og = fOGoods[i];

            if(og.rowDiscount) {
                discountRow = true;
                break;
            }
        }

        for(int i = 0; i < fOGoods.count(); i++) {
            OGoods &og = fOGoods[i];

            if(discountRow) {
                if(og.rowDiscount) {
                    og.discountFactor = fBHistory.value / 100;
                    og.discountMode = fBHistory.type;
                }
            } else {
                og.discountFactor = fBHistory.value / 100;
                og.discountMode = fBHistory.type;
            }
        }

        countTotal();
    }, [this, code](const QJsonObject & jerr) {
        Q_UNUSED(jerr);
        processCode(code, -1, [this, code](const QString & c) {checkGoodsCode(c);});
    });
}

void WOrder::checkGoodsCode(const QString &code)
{
    if(fOHeader.saleType == -1) {
        if(fOGoods.count() > 0) {
            C5Message::error(tr("Cannot add goods in prepaid mode"));
            return;
        }
    }

    fHttp->createHttpQueryLambda("/engine/v2/shop/process-barcode/get", {
        {"barcode", code},
        {"store", __c5config.defaultStore()},
        {"draft_header", fDraftSale._id()},
        {"retail", fOHeader.saleType == SALE_RETAIL}
    },
    [this, code](const QJsonObject & jdoc) {
        QJsonObject goods = jdoc["goods"].toObject();
        QJsonObject store = jdoc["store"].toObject();
        double price = 0;

        if(ui->tblData->rowCount() > 0) {
            if(!goods["f_autodiscount"].toString().isEmpty() && !__c5config.fMainJson["shop_autodiscount_card_number"].toString().isEmpty()) {
                fHttp->httpQueryFinished(sender());
                C5Message::error(tr("Only one item can be added to the special sale"));
                return;
            } else {
            }
        }

        switch(fOHeader.saleType) {
        case SALE_RETAIL:
            price = goods["f_price1"].toDouble();
            break;

        case SALE_WHOSALE:
            price = goods["f_price2"].toDouble();;
            break;

        default:
            price = goods["f_price1"].toDouble();
            break;
        }

        QJsonObject jm = sender()->property("marks").toJsonObject();

        if(jm.contains("price")) {
            price = jm["price"].toDouble();
        }

        int row = ui->tblData->addEmptyRow();
        auto *ch = new C5CheckBox();
        ch->setCheckable(s.value("learnaccumulate").toBool());
        ch->setChecked(goods["f_candiscount"].toInt() == 1);
        connect(ch, &C5CheckBox::clicked, this, &WOrder::checkCardClicked);
        ui->tblData->setCellWidget(row, col_check_discount, ch);
        OGoods og;
        double qty = goods["f_defaultqty"].toDouble();

        if(jdoc.contains("23")) {
            auto qtyStr = code.right(6);
            qtyStr.removeLast();
            qty = qtyStr.toDouble() / 1000;
        }

        og._groupName = goods["f_groupname"].toString();
        og._goodsName = goods["f_name"].toString();
        og._goodsFiscalName = goods["f_fiscalname"].toString();
        og._unitName = goods["f_unitname"].toString();
        og._barcode = goods["f_scancode"].toString();
        og.header = fOHeader._id();
        og.goods = goods["f_id"].toInt();
        og.taxDept = goods["f_taxdept"].toInt();
        og.adgCode = goods["f_adgcode"].toString();
        og.isService = goods["f_service"].toInt();
        og.qty = qty;
        og.price = price;
        og.store = __c5config.defaultStore();
        og.total = og.qty * og.price;
        og.discountFactor = fBHistory.value / 100;
        og.discountMode = fBHistory.type;
        og.discountAmount = 0;
        og.emarks = jdoc["emarks"].toString();
        og.canDiscount = goods["f_candiscount"].toInt();
        fOGoods.append(og);
        ui->tblData->setDouble(row, col_stock, store["f_qty"].toDouble());
        ui->tblData->item(row, 0)->setData(Qt::UserRole + 101, jdoc["draftid"].toString());
        ui->tblData->setCurrentCell(row, 0);
        countTotal();
        ui->leCode->setFocus();
    }, [this](const QJsonObject jerr) {
        ui->leCode->setFocus();
    });
}

void WOrder::processPresentCard(const QString & code)
{
//SALE GIFT CARD
    QJsonObject jdoc;
    QDate validDate = QDate::fromString(jdoc["card"].toObject()["f_datesaled"].toString(), FORMAT_DATE_TO_STR_MYSQL);

    if(validDate.isValid()) {
        if(validDate.addDays(31 * 6) < QDate::currentDate()) {
            fHttp->httpQueryFinished(sender());
            C5Message::error(tr("Card not valid"));
            return;
        }
    }

    // ui->lbCardDateEnd->setVisible(true);
    // ui->lbCardDateEnd->setText(validDate.addDays(31 * 6).toString(FORMAT_DATE_TO_STR));
    // if(fOHeader.saleType == -1) {
    //     if(fOGoods.count() > 0) {
    //         C5Message::error(tr("Cannot add goods in prepaid mode"));
    //         break;
    //     }
    // }
    fGiftCard = jdoc["card"].toObject()["f_id"].toInt();
    // if(jdoc["used"].toDouble() > 0) {
    //     ui->leGiftCardAmount->setDouble(jdoc["used"].toDouble());
    //     fHttp->httpQueryFinished(sender());
    //     return;
    // } else {
    //     if(jdoc["card"].toObject()["f_datesaled"].toString().isEmpty() == false) {
    //         C5Message::error(tr("Card balance is 0"));
    //         fHttp->httpQueryFinished(sender());
    //         return;
    //     }
    // }
    QJsonObject goods = jdoc["goods"].toObject();
    QJsonObject store = jdoc["store"].toObject();
    double price = 0;
    QJsonObject jm = sender()->property("marks").toJsonObject();

    switch(fOHeader.saleType) {
    case SALE_RETAIL:
        price = goods["f_price1"].toDouble();
        break;

    case SALE_WHOSALE:
        price = goods["f_price2"].toDouble();;
        break;

    default:
        price = goods["f_price1"].toDouble();
        break;
    }

    if(!jm.isEmpty()) {
        price = jm["price"].toDouble();
    }

    int row = ui->tblData->addEmptyRow();
    auto *ch = new C5CheckBox();
    ch->setCheckable(s.value("learnaccumulate").toBool());
    ch->setChecked(goods["f_candiscount"].toInt() == 1);
    connect(ch, &C5CheckBox::clicked, this, &WOrder::checkCardClicked);
    ui->tblData->setCellWidget(row, col_check_discount, ch);
    OGoods og;
    og._groupName = goods["f_groupname"].toString();
    og._goodsName = goods["f_name"].toString();
    og._goodsFiscalName = goods["f_fiscalname"].toString();
    og._unitName = goods["f_unitname"].toString();
    og._barcode = goods["f_scancode"].toString();
    og.header = fOHeader._id();
    og.goods = goods["f_id"].toInt();
    og.taxDept = goods["f_taxdept"].toInt();
    og.adgCode = goods["f_adgcode"].toString();
    og.isService = goods["f_service"].toInt();
    og.qty = goods["f_defaultqty"].toDouble();
    og.price = price;
    og.store = __c5config.defaultStore();
    og.total = og.qty * og.price;
    og.discountFactor = fBHistory.value / 100;
    og.discountMode = fBHistory.type;
    og.discountAmount = 0;
    og.emarks = jdoc["emarks"].toString();
    og.canDiscount = goods["f_candiscount"].toInt();
    fOGoods.append(og);
    ui->tblData->setDouble(row, col_stock, store["f_qty"].toDouble());
    countTotal();
    ui->tblData->item(row, 0)->setData(Qt::UserRole + 101, jdoc["draftid"].toString());
}

void WOrder::processAccumulateCard(const QString & code)
{
    // if(card["f_mode"].toInt() == CARD_TYPE_ACCUMULATIVE) {
    //     ui->leDisc->setText(QString("%1%").arg(float_str(card["f_value"].toDouble(), 2)));
    //     ui->leDisc->setVisible(true);
    //     ui->lbDisc->setVisible(true);
    //     //TODO: ui->leGiftCardAmount->setDouble(history["f_amount"].toDouble());
    //     //TODO: ui->leUseAccumulated->setReadOnly(!(ui->leGiftCardAmount->getDouble() > 0));
    //     ui->lbUseAccumulated->setVisible(true);
    //     ui->leUseAccumulated->setVisible(true);
    // }
}
