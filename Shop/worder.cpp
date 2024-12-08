#include "worder.h"
#include "outputofheader.h"
#include "ui_worder.h"
#include "c5database.h"
#include "c5config.h"
#include "printtaxn.h"
#include "imageloader.h"
#include "dqty.h"
#include "c5message.h"
#include "c5permissions.h"
#include "c5printing.h"
#include "printreceiptgroup.h"
#include "printreceipt.h"
#include "selectstaff.h"
#include "c5shoporder.h"
#include "dlgsearchpartner.h"
#include "working.h"
#include "c5replacecharacter.h"
#include "selectprinters.h"
#include "dlgpaymentchoose.h"
#include "c5user.h"
#include "datadriver.h"
#include "wcustomerdisplay.h"
#include "c5utils.h"
#include "QRCodeGenerator.h"
#include "c5checkbox.h"
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
    fDraftSale.saleType = saleType;
    fDraftSale.state = 1;
    fDraftSale.date = QDate::currentDate();
    fDraftSale.time = QTime::currentTime();
    fDraftSale.staff = user->id();
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
    fWorking = static_cast<Working *>(parent);
    ui->lbDisc->setVisible(false);
    ui->leDisc->setVisible(false);
    ui->lbUseAccumulated->setVisible(false);
    ui->leUseAccumulated->setVisible(false);
    noImage();
    ui->btnF1->setVisible(fWorking->fFlags.contains(1));
    ui->btnF2->setVisible(fWorking->fFlags.contains(2));
    ui->btnF3->setVisible(fWorking->fFlags.contains(3));
    ui->btnF4->setVisible(fWorking->fFlags.contains(4));
    ui->btnF5->setVisible(fWorking->fFlags.contains(5));
    ui->btnF1->setText(fWorking->flag(1).name);
    ui->btnF2->setText(fWorking->flag(2).name);
    ui->btnF3->setText(fWorking->flag(3).name);
    ui->btnF4->setText(fWorking->flag(4).name);
    ui->btnF5->setText(fWorking->flag(5).name);
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
    ui->btnF3->setVisible(false);
    ui->btnF4->setVisible(false);
    ui->btnF5->setVisible(false);
    C5Database db;
    QString err;
    fDraftSale.write(db, err);
#ifdef BF10
    addGoods("10BF");
#endif
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
    if (fCustomerDisplay) {
        fCustomerDisplay->clear();
        for (int i = 0; i < fOGoods.count(); i++) {
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
    if (ui->leCode->text().length() > 1) {
        return;
    }
    clearCode();
    removeRow();
}

void WOrder::keyPlus()
{
    if (ui->leCode->text().length() > 1) {
        return;
    }
    clearCode();
    if (__c5config.getValue(param_shop_deny_qtychange).toInt() == 0) {
        changeQty();
    }
}

void WOrder::keyAsterix()
{
    if (ui->leCode->text().length() > 1) {
        return;
    }
    clearCode();
    changePrice();
}

void WOrder::focusTaxpayerId()
{
    ui->leCustomerTaxpayerId->setFocus();
    ui->leCustomerTaxpayerId->setSelection(0, 1000);
}

void WOrder::imageConfig()
{
    int levelIndex = 1;
    int versionIndex = 0;
    bool bExtent = true;
    int maskIndex = -1;
    QString encodeString = __c5config.getValue(param_shop_config_mobile_client);
    CQR_Encode qrEncode;
    bool successfulEncoding = qrEncode.EncodeData(levelIndex, versionIndex, bExtent, maskIndex,
                              encodeString.toUtf8().data() );
    if (!successfulEncoding) {
        //fLog.append("Cannot encode qr image");
    }
    int qrImageSize = qrEncode.m_nSymbleSize;
    int encodeImageSize = qrImageSize + ( QR_MARGIN * 2 );
    QImage encodeImage(encodeImageSize, encodeImageSize, QImage::Format_Mono);
    encodeImage.fill(1);
    for ( int i = 0; i < qrImageSize; i++ ) {
        for ( int j = 0; j < qrImageSize; j++ ) {
            if ( qrEncode.m_byModuleData[i][j] ) {
                encodeImage.setPixel(i + QR_MARGIN, j + QR_MARGIN, 0);
            }
        }
    }
    QPixmap pix = QPixmap::fromImage(encodeImage).scaled(500, 500);
    ui->lbGoodsImage->setPixmap(pix);
}

bool WOrder::writeOrder()
{
    if (fOGoods.count() == 0) {
        C5Message::error(tr("Empty order"));
        return false;
    }
    if (__c5config.cashId() == 0) {
        C5Message::error(tr("Cashdesk for cash not defined"));
        return false;
    }
    if (__c5config.nocashId() == 0) {
        C5Message::error(tr("Cashdesk for card not defined"));
        return false;
    }
    bool prepaidReadonly = false;
    if (ui->leUseAccumulated->getDouble() > 0) {
        fOHeader.amountPrepaid = ui->leUseAccumulated->getDouble();
        prepaidReadonly = true;
    }
    if (fOHeader.amountCash < 0.001
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
    if (ui->btnF5->isChecked()) {
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
    if (!DlgPaymentChoose::getValues(fOHeader.amountTotal, fOHeader.amountCash, fOHeader.amountCard, fOHeader.amountIdram,
                                     fOHeader.amountTelcell, fOHeader.amountBank, fOHeader.amountCredit,
                                     fOHeader.amountPrepaid, fOHeader.amountDebt,
                                     fOHeader.amountCashIn, fOHeader.amountChange, fOHeader._printFiscal, prepaidReadonly,
                                     ui->leGiftCardAmount->getDouble())) {
        return false;
    }
    QElapsedTimer t;
    t.start();
    if ((fOHeader.amountDebt > 0.001 || fOHeader.amountBank > 0.001) && fOHeader.partner == 0) {
        C5Message::error(tr("Debt impossible on unknown partner"));
        return false;
    }
    for (int i = 0; i < fOGoods.count(); i++) {
        const OGoods &g = fOGoods.at(i);
        if (g.qty < 0.0001) {
            C5Message::error(tr("Invalid qty"));
            return false;
        }
        if (g.price < 0) {
            C5Message::error(tr("Invalid price"));
            return false;
        }
    }
    C5User *u = fUser;
    SelectStaff ss(fWorking);
    if (__c5config.shopDifferentStaff() && fWorking->fCurrentUsers.count() > 0) {
        if (ss.exec() == QDialog::Rejected) {
            return false;
        }
        u = ss.fUser;
        if (!u->isValid()) {
            return false;
        }
    }
    QJsonObject jdoc;
    jdoc["session"] = C5Database::uuid();
    jdoc["giftcard"] = fGiftCard;
    jdoc["settings"] = __c5config.fSettingsName;
    jdoc["organization"] = ui->leOrganization->text();
    jdoc["contact"] = ui->leContact->text();
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
    jh["f_staff"] = u->id();
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
    for (int i = 0; i < fOGoods.count(); i++) {
        OGoods &g = fOGoods[i];
        g.price = g.total / (g.qty / g._qtybox);
        g.qty /= g._qtybox;
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
        jt["f_row"] = g.row;
        jt["f_storerec"] = g.storeRec;
        jt["f_discountfactor"] = g.discountFactor;
        jt["f_discountmode"] = g.discountMode;
        jt["f_discountamount"] = g.discountAmount;
        jt["f_return"] = g.return_;
        jt["f_returnfrom"] = g.returnFrom;
        jt["f_isservice"] = g.isService;
        jt["f_amountaccumulate"] = g.accumulateAmount;
        jt["f_emarks"] = QString(g.emarks).replace("\"", "\\\"");
        jg.append(jt);
    }
    jdoc["goods"] = jg;
    QJsonObject jhistory;
    if (fBHistory.card > 0 && fBHistory.type == CARD_TYPE_ACCUMULATIVE) {
        QJsonObject jtemp;
        jtemp["card"] = fBHistory.card;
        jtemp["accumulate"] = ui->leCurrentAccumulated->getDouble();
        jtemp["accumulatespend"] = ui->leUseAccumulated->getDouble();
        jdoc["accumulate"] = jtemp;
    }
    jhistory["f_card"] = fBHistory.card;
    jhistory["f_data"] = fBHistory.data;
    jhistory["f_type"] = fBHistory.type;
    jdoc["history"] = jhistory;
    QJsonObject jflags;
    jflags["f_1"] = ui->btnF1->isChecked() ? 1 : 0;
    jflags["f_2"] = ui->btnF2->isChecked() ? 1 : 0;
    jflags["f_3"] = ui->btnF3->isChecked() ? 1 : 0;
    jflags["f_4"] = ui->btnF4->isChecked() ? 1 : 0;
    jflags["f_5"] = ui->btnF5->isChecked() ? 1 : 0;
    jdoc["flags"] = jflags;
    QString sql = QString(QJsonDocument(jdoc).toJson(QJsonDocument::Compact));
    sql.replace("'", "\\'");
    sql = QString("call sf_create_shop_order('%1')").arg(sql);
    C5Database db(__c5config.dbParams());
    if (!db.exec(sql)) {
        C5Message::error(db.fLastError);
        return false;
    }
    db[":f_session"] = jdoc["session"].toString();
    if (!db.exec("select f_result from a_result where f_session=:f_session")) {
        C5Message::error(db.fLastError);
        return false;
    }
    if (!db.nextRow()) {
        C5Message::error("Program error. Cannot get result of session");
        return false;
    }
    QJsonObject jr = QJsonDocument::fromJson(db.getString("f_result").toUtf8()).object();
    if (jr["status"].toInt() != 1) {
        C5Message::error(jr["message"].toString());
        return false;
    }
    if (C5Config::taxIP().isEmpty()) {
        fOHeader._printFiscal = false;
    }
    if (fOHeader._printFiscal) {
        QElapsedTimer et;
        et.restart();
        PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(),
                     fOHeader.hasIdram() ? "true" : C5Config::taxUseExtPos(), C5Config::taxCashier(), C5Config::taxPin(), this);
        QString jsonIn, jsonOut, err;
        int result = 0;
        if (fOHeader.partner > 0) {
            pt.fPartnerTin = fOHeader.taxpayerTin;
        }
        if (fOHeader.saleType != -1) {
            for (int i = 0; i < fOGoods.count(); i++) {
                OGoods &g = fOGoods[i];
                if (g.emarks.isEmpty() == false) {
                    pt.fEmarks.append(g.emarks);
                }
                double price = g.price / g._qtybox;
                if (fBHistory.value > 0.01) {
                    price = price / (100 - (fBHistory.value )) * 100;
                }
                pt.addGoods(g.taxDept, //dep
                            g.adgCode, //adg
                            QString::number(g.goods), //goods id
                            g._goodsFiscalName.isEmpty() ? g._goodsName : g._goodsFiscalName, //name
                            price, //price
                            g.qty, //qty
                            fBHistory.value); //discount
            }
            do {
                result = pt.makeJsonAndPrint(fOHeader.amountCard + fOHeader.amountIdram + fOHeader.amountTelcell,
                                             fOHeader.amountPrepaid, jsonIn, jsonOut, err);
                if (result != pt_err_ok) {
                    switch (C5Message::question(err, tr("Try again"), tr("Do not print fiscal"), tr("Return to editing"))) {
                        case QDialog::Rejected:
                            break;
                        case QDialog::Accepted:
                            result = pt_err_ok;
                            break;
                        case 2:
                            return false;
                    }
                }
            } while (result != pt_err_ok);
        } else {
            result = pt.printAdvanceJson(fOHeader.amountCash, fOHeader.amountCard, jsonIn, jsonOut, err);
        }
        QJsonParseError jerr;
        QJsonDocument jod = QJsonDocument::fromJson(jsonOut.toUtf8(), &jerr);
        if (jerr.error != QJsonParseError::NoError) {
            err = jerr.errorString();
            jod = QJsonDocument::fromJson(QString("{\"data\":\"" + jsonOut + "\"").toUtf8(), &jerr);
        }
        QDir dir = QDir::tempPath();
        QFile file(dir.tempPath() + "/printtax_errors.txt");
        if (file.open(QIODevice::Append)) {
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
        if (result != pt_err_ok) {
            C5Message::error(err);
        }
    }
    if (!C5Config::localReceiptPrinter().isEmpty()) {
        PrintReceiptGroup p;
        switch (C5Config::shopPrintVersion()) {
            case 1: {
                bool p1, p2;
                if (SelectPrinters::selectPrinters(p1, p2)) {
                    if (p1) {
                        p.print(fOHeader._id(), db, 1);
                    }
                    if (p2) {
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
    if (fOHeader.partner > 0) {
        OutputOfHeader ooh;
        ooh.make(db, fOHeader._id());
    }
    if (!fDraftSale.id.toString().isEmpty()) {
        db[":f_id"] = fDraftSale.id;
        db.exec("update o_draft_sale set f_state=3 where f_id=:f_id");
        db[":f_header"] = fDraftSale.id;
        db.exec("update o_draft_sale_body set f_state=3 where f_header=:f_header");
    }
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
    fOHeader.partner = db.getInt("f_client");
    fBHistory.card = db.getInt("f_id");
    fBHistory.type = db.getInt("f_mode");
    fBHistory.value = db.getDouble("f_value");
    db[":f_id"] = fOHeader.partner;
    db.exec("select * from c_partners where f_id=:f_id");
    if (!db.nextRow()) {
        return;
    }
    if (__c5config.getValue(param_auto_discount) != code) {
        if (!checkDiscountRight()) {
            return;
        }
    }
    if (fBHistory.value < 0) {
        double v;
        if (!getDiscountValue(fBHistory.type, v)) {
            fBHistory.card = 0;
            fBHistory.value = 0;
            fBHistory.data = 0;
            return;
        }
        fBHistory.value = v;
        if (fBHistory.type == CARD_TYPE_DISCOUNT) {
            fBHistory.value = v / 100;
        } else {
            fBHistory.value = v;
        }
    }
    for (int i = 0; i < fOGoods.count(); i++ ) {
        OGoods &g = fOGoods[i];
        g.discountMode = fBHistory.type;
        g.discountFactor = fBHistory.value;
    }
    ui->leOrganization->setText(db.getString("f_taxinfo"));
    ui->leContact->setText(db.getString("f_contact"));
    countTotal();
}

void WOrder::changeQty()
{
    int row = ui->tblData->currentRow();
    if (row < 0) {
        return;
    }
    if (fOGoods.at(row).discountMode == CARD_TYPE_MANUAL) {
        C5Message::error(tr("Cannot change the quantity on selected row with manual discount mode"));
        return;
    }
    double qty = DQty::getQty(tr("Quantity"), 0, this);
    if (qty < 0.001) {
        return;
    }
    setQtyOfRow(row, qty);
}

void WOrder::changeQty(double qty)
{
    int row = ui->tblData->currentRow();
    if (row < 0) {
        return;
    }
    if (fOGoods.at(row).discountMode == CARD_TYPE_MANUAL) {
        C5Message::error(tr("Cannot change the quantity on selected row with manual discount mode"));
        return;
    }
    if (qty < 0.001) {
        return;
    }
    setQtyOfRow(row, qty);
}

void WOrder::discountRow(const QString &code)
{
    if (!checkDiscountRight()) {
        return;
    }
    int row = ui->tblData->currentRow();
    if (row < 0) {
        return;
    }
    OGoods &g = fOGoods[row];
    if (g.discountFactor > 0.001) {
        C5Message::error(tr("Discount already applied"));
        return;
    }
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
    double v = db.getDouble("f_value") / 100;
    int discType = db.getInt("f_mode");
    if (db.getDouble("f_value") < 0) {
        if (!getDiscountValue(discType, v)) {
            return;
        }
        if (discType == CARD_TYPE_DISCOUNT) {
            v /= 100;
        }
    }
    g.discountMode = discType;
    switch (discType) {
        case CARD_TYPE_DISCOUNT:
            g.discountFactor = v;
            g.discountMode = discType;
            break;
        case CARD_TYPE_MANUAL:
            if (v > g.total) {
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
    if (row < 0) {
        return;
    }
    OGoods &g = fOGoods[row];
    double currentPrice = g.price;
    if (__c5config.shopDenyPriceChange() && !(currentPrice < 0)) {
        return;
    }
    double price = DQty::getQty(tr("Price"), 0, this);
    if (price < 0.001) {
        return;
    }
    g.price = price;
    C5Database db(__c5config.dbParams());
    db[":f_id"] = ui->tblData->item(row, 0)->data(Qt::UserRole + 101);
    db[":f_price"] = g.price;
    db.exec("update o_draft_sale_body set f_price=:f_price where f_id=:f_id");
    countTotal();
}

void WOrder::changePrice(double price)
{
    int row = ui->tblData->currentRow();
    if (row < 0) {
        return;
    }
    fOGoods[row].price = price;
    C5Database db(__c5config.dbParams());
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
    if (row < 0) {
        return;
    }
    C5User *tmp = fUser;
    if (!tmp->check(cp_t5_remove_row_from_shop)) {
        QString password = QInputDialog::getText(this, tr("Password"), tr("Password"), QLineEdit::Password);
        C5User *tmp = new C5User(password);
        if (tmp->error().isEmpty()) {
        } else {
            C5Message::error(tmp->error());
            delete tmp;
            return;
        }
    }
    OGoods &g = fOGoods[row];
    QString name = g._goodsName;
    C5Database db(__c5config.dbParams());
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
    if (row + 1 < ui->tblData->rowCount()) {
        ui->tblData->setCurrentItem(ui->tblData->item(row + 1, 0));
    }
}

void WOrder::prevRow()
{
    int row = ui->tblData->currentRow();
    if (row - 1 > -1) {
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
    if (__c5config.getValue(param_shop_deny_qtychange).toInt() == 0) {
        int row = ui->tblData->currentRow();
        if (row < 0) {
            return;
        }
        if (fOGoods.at(row).discountMode == CARD_TYPE_MANUAL) {
            C5Message::error(tr("Cannot change the quantity on selected row with manual discount mode"));
            return;
        }
        double qty = DQty::getQty(tr("Box"), 0, this);
        if (qty < 0.001) {
            return;
        }
        setQtyOfRow(row, qty *fOGoods.at(row)._qtybox);
    }
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
    double accAmount = 0;
    fOHeader.countAmount(fOGoods, fBHistory);
    ui->leTotal->setDouble(fOHeader.amountTotal);
    if (fBHistory.type == CARD_TYPE_DISCOUNT) {
        ui->leDisc->setDouble(fOHeader.amountDiscount);
    }
    for (int i = 0; i < fOGoods.count(); i++) {
        const OGoods &og = fOGoods.at(i);
        ui->tblData->setData(i, col_bacode, og._barcode);
        ui->tblData->setData(i, col_group, og._groupName);
        ui->tblData->setData(i, col_name, og._goodsName);
        ui->tblData->setData(i, col_qty, og.qty);
        ui->tblData->setData(i, col_qtybox, og.qty / og._qtybox);
        ui->tblData->setData(i, col_price, og.price);
        ui->tblData->setData(i, col_unit, og._unitName);
        ui->tblData->setData(i, col_total, og.total);
        ui->tblData->setData(i, col_discamount, og.discountAmount);
        ui->tblData->setData(i, col_discvalue, og.discountFactor);
        ui->tblData->setData(i, col_discmode, og.discountMode);
        QPushButton *btn = static_cast<QPushButton *>(ui->tblData->cellWidget(i, col_qr));
        if (btn == nullptr ) {
            btn = new QPushButton();
        }
        if (og.emarks.isEmpty()) {
            btn->setIcon(QPixmap(""));
        } else {
            btn->setIcon(QPixmap(":/qr_code.png").scaled(30, 30));
        }
        connect(btn, &QPushButton::clicked, this, &WOrder::readEmarks);
        ui->tblData->setCellWidget(i, col_qr, btn);
        if (fBHistory.card > 0 && fBHistory.type == CARD_TYPE_ACCUMULATIVE) {
            auto *ch = static_cast<C5CheckBox *>(ui->tblData->cellWidget(i, col_check_discount));
            if (ch->isChecked()) {
                accAmount += og.total *fBHistory.value;
            }
        }
    }
    ui->leCurrentAccumulated->setDouble(accAmount);
    ui->leTotal->setDouble(fOHeader.amountTotal);
    C5Database db(__c5config.dbParams());
    QString err;
    fDraftSale.amount = fOHeader.amountTotal;
    fDraftSale.write(db, err);
    updateCustomerDisplay(fCustomerDisplay);
}

bool WOrder::returnFalse(const QString &msg, C5Database &db)
{
    db.rollback();
    db.close();
    C5Message::error(msg);
    return false;
}

bool WOrder::getDiscountValue(int discountType, double &v)
{
    bool ok = true;
    QString disctitle;
    double maxvalue = 100;
    switch (discountType) {
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
    if (!fDraftSale._id().isEmpty() && ui->tblData->rowCount() == 0) {
        C5Database db;
        db[":f_id"] = fDraftSale._id();
        db.exec("update o_draft_sale set f_state=3 where f_id=:f_id");
        db[":f_header"] = fDraftSale._id();
        db.exec("update o_draft_sale_body set f_state=3 where f_header=:f_header");
    }
}

bool WOrder::checkDiscountRight()
{
    C5User *u = fUser;
    if (!u->check(cp_t5_discount)) {
        QString password = QInputDialog::getText(this, tr("Password"), tr("Password"), QLineEdit::Password);
        C5User *tmp = new C5User(password);
        if (tmp->error().isEmpty()) {
            if (!tmp->check(cp_t5_discount)) {
                delete tmp;
                C5Message::error(tr("Access denied"));
                return false;
            }
            return true;
        } else {
            C5Message::error(tmp->error());
            delete tmp;
            return false;
        }
    }
    return true;
}

bool WOrder::setQtyOfRow(int row, double qty)
{
    OGoods &og = fOGoods[row];
    DbGoods g(og.goods);
    if (g.acceptIntegerQty()) {
        qty = trunc(qty);
    }
    double diffqty = (qty / og._qtybox);
    if (og.isService == 0  && __c5config.getValue(param_shop_dont_check_qty).toInt() == 0) {
        QString err;
        if (!checkQty(g.id(), diffqty, err, og.qty)) {
            C5Message::error(og._goodsName + " \r\n" + err);
            return false;
        }
    }
    og.qty = qty;
    C5Database db(__c5config.dbParams());
    db[":f_id"] = ui->tblData->item(row, 0)->data(Qt::UserRole + 101);
    db[":f_qty"] = og.qty / og._qtybox;
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

C5ClearTableWidget *WOrder::table()
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
    C5Database db(__c5config.dbParams());
    db.exec(QString("select sf_check_goods_qty('%1')").arg(json_str(params)));
    if (db.nextRow()) {
        params = str_json(db.getString(0));
        if (params["status"].toInt() == 0) {
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

void WOrder::addGoods(const QString &barcode)
{
    fHttp->createHttpQuery("/engine/shop/process-barcode.php",
    QJsonObject{{"code", barcode},
        {"store", __c5config.defaultStore()},
        {"draft_header", fDraftSale._id()},
        {"retail", fOHeader.saleType == SALE_RETAIL}},
    SLOT(reponseProcessCode(QJsonObject)));
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

void WOrder::reponseProcessCode(const QJsonObject &jdoc)
{
    switch (jdoc["barcode"].toInt()) {
        case 1: {
            //GIFT CARD
            QJsonObject card = jdoc["card"].toObject();
            if (card["f_mode"].toInt() == CARD_TYPE_DISCOUNT) {
                if (__c5config.getValue(param_auto_discount) != card["f_code"].toString()) {
                    if (!checkDiscountRight()) {
                        fHttp->httpQueryFinished(sender());
                        return;
                    }
                }
            }
            QJsonObject history = jdoc["history"].toObject();
            QJsonObject partner = jdoc["partner"].toObject();
            ui->leContact->setText(partner["f_contact"].toString());
            ui->leOrganization->setText(partner["f_taxname"].toString());
            fBHistory.card = card["f_id"].toInt();
            fBHistory.type = card["f_mode"].toInt();
            fBHistory.value = card["f_value"].toDouble();
            if (card["f_mode"].toInt() == CARD_TYPE_DISCOUNT) {
                if (fBHistory.value < 0) {
                    double v = fBHistory.value;
                    if (!getDiscountValue(CARD_TYPE_DISCOUNT, v)) {
                        return;
                    }
                    //v /= 100;
                    fBHistory.value = v;
                }
                ui->leDisc->setText(QString("%1%").arg(float_str(card["f_value"].toDouble(), 2)));
                ui->leDisc->setVisible(true);
                ui->lbDisc->setVisible(true);
                ui->leGiftCardAmount->setDouble(history["f_amount"].toDouble());
                ui->leContact->setText(partner["f_name"].toString());
                for (int i = 0; i < fOGoods.count(); i++) {
                    OGoods &og = fOGoods[i];
                    og.discountFactor = fBHistory.value / 100;
                    og.discountMode = fBHistory.type;
                }
                countTotal();
            } else if (card["f_mode"].toInt() == CARD_TYPE_ACCUMULATIVE) {
                ui->leDisc->setText(QString("%1%").arg(float_str(card["f_value"].toDouble(), 2)));
                ui->leDisc->setVisible(true);
                ui->lbDisc->setVisible(true);
                ui->leGiftCardAmount->setDouble(history["f_amount"].toDouble());
                ui->leContact->setText(partner["f_name"].toString());
                ui->leUseAccumulated->setReadOnly(!(ui->leGiftCardAmount->getDouble() > 0));
                ui->lbUseAccumulated->setVisible(true);
                ui->leUseAccumulated->setVisible(true);
            }
            countTotal();
            break;
        }
        case 2: {
            if (fOHeader.saleType == -1) {
                if (fOGoods.count() > 0) {
                    C5Message::error(tr("Cannot add goods in prepaid mode"));
                    break;
                }
            }
            QJsonObject goods = jdoc["goods"].toObject();
            QJsonObject store = jdoc["store"].toObject();
            double price = 0;
            QJsonObject jm = sender()->property("marks").toJsonObject();
            switch (fOHeader.saleType) {
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
            if (!jm.isEmpty()) {
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
            og._qtybox = goods["f_qtybox"].toDouble();
            og.header = fOHeader._id();
            og.goods = goods["f_id"].toInt();
            og.taxDept = goods["f_taxdept"].toInt();
            og.adgCode = goods["f_adgcode"].toString();
            og.isService = goods["f_service"].toInt();
            og.qty = goods["f_defaultqty"].toDouble();
            og.price = price;
            og.store = __c5config.defaultStore();
            og.total = og.qty *og.price;
            og.discountFactor = fBHistory.value / 100;
            og.discountMode = fBHistory.type;
            og.discountAmount = 0;
            og.emarks = jdoc["emarks"].toString();
            og.canDiscount = goods["f_candiscount"].toInt();
            fOGoods.append(og);
            ui->tblData->setCurrentCell(row, 0);
            //ui->tblData->setDouble(row, col_stock, (g.unit()->defaultQty()/og._qtybox) + (qtyStore / og._qtybox));
            ui->tblData->setDouble(row, col_stock, (store["f_qty"].toDouble() / og._qtybox));
            countTotal();
            ui->tblData->item(row, 0)->setData(Qt::UserRole + 101, jdoc["draftid"].toString());
            ImageLoader *il = new ImageLoader(goods["f_id"].toInt(), this);
            connect(il, SIGNAL(imageLoaded(QPixmap)), this, SLOT(imageLoaded(QPixmap)));
            connect(il, SIGNAL(noImage()), this, SLOT(noImage()));
            il->start();
        }
        break;
        //SALE GIFT CARD
        case 3: {
            if (fOHeader.saleType == -1) {
                if (fOGoods.count() > 0) {
                    C5Message::error(tr("Cannot add goods in prepaid mode"));
                    break;
                }
            }
            fGiftCard = jdoc["card"].toObject()["f_id"].toInt();
            if (jdoc["used"].toDouble() > 0) {
                ui->leGiftCardAmount->setDouble(jdoc["used"].toDouble());
                fHttp->httpQueryFinished(sender());
                return;
            } else {
                if ( jdoc["card"].toObject()["f_datesaled"].toString().isEmpty() == false) {
                    C5Message::error(tr("Card balance is 0"));
                    fHttp->httpQueryFinished(sender());
                    return;
                }
            }
            ui->leOrganization->setReadOnly(false);
            ui->leContact->setReadOnly(false);
            ui->lbPartner->setText(tr("Costumer"));
            ui->lbContact->setText(tr("Phone"));
            QJsonObject goods = jdoc["goods"].toObject();
            QJsonObject store = jdoc["store"].toObject();
            double price = 0;
            QJsonObject jm = sender()->property("marks").toJsonObject();
            switch (fOHeader.saleType) {
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
            if (!jm.isEmpty()) {
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
            og._qtybox = goods["f_qtybox"].toDouble();
            og.header = fOHeader._id();
            og.goods = goods["f_id"].toInt();
            og.taxDept = goods["f_taxdept"].toInt();
            og.adgCode = goods["f_adgcode"].toString();
            og.isService = goods["f_service"].toInt();
            og.qty = goods["f_defaultqty"].toDouble();
            og.price = price;
            og.store = __c5config.defaultStore();
            og.total = og.qty *og.price;
            og.discountFactor = fBHistory.value / 100;
            og.discountMode = fBHistory.type;
            og.discountAmount = 0;
            og.emarks = jdoc["emarks"].toString();
            og.canDiscount = goods["f_candiscount"].toInt();
            fOGoods.append(og);
            ui->tblData->setCurrentCell(row, 0);
            //ui->tblData->setDouble(row, col_stock, (g.unit()->defaultQty()/og._qtybox) + (qtyStore / og._qtybox));
            ui->tblData->setDouble(row, col_stock, (store["f_qty"].toDouble() / og._qtybox));
            countTotal();
            ui->tblData->item(row, 0)->setData(Qt::UserRole + 101, jdoc["draftid"].toString());
            ImageLoader *il = new ImageLoader(goods["f_id"].toInt(), this);
            connect(il, SIGNAL(imageLoaded(QPixmap)), this, SLOT(imageLoaded(QPixmap)));
            connect(il, SIGNAL(noImage()), this, SLOT(noImage()));
            il->start();
        }
        break;
    }
    fHttp->httpQueryFinished(sender());
}

void WOrder::imageLoaded(const QPixmap &img)
{
    ui->lbGoodsImage->setPixmap(img.scaled(ui->lbGoodsImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    //ui->wimage->setVisible(true);
}

void WOrder::openDraftResponse(const QJsonObject &jdoc)
{
    removeDraft();
    QJsonObject js = jdoc["ds"].toObject();
    fDraftSale.saleType = js["f_saletype"].toInt();
    fDraftSale.id = js["f_id"].toString();
    QJsonArray ja = jdoc["goods"].toArray();
    ui->tblData->setRowCount(0);
    fOGoods.clear();
    for (int i = 0; i < ja.size(); i++) {
        QJsonObject jo = ja.at(i).toObject();
        OGoods og;
        DbGoods g(jo["f_goods"].toInt());
        og._groupName = g.group()->groupName();
        og._goodsName = g.goodsName();
        og._goodsFiscalName = g.goodsFiscalName();
        og._unitName = g.unit()->unitName();
        og._barcode = g.scancode();
        og._qtybox = g.qtyBox();
        og.header = fOHeader._id();
        og.goods = jo["f_goods"].toInt();
        og.taxDept = g.group()->taxDept();
        og.adgCode = g.group()->adgt();
        og.isService = g.isService();
        og.qty = jo["f_qty"].toDouble();
        og.price = fOHeader.saleType == SALE_RETAIL ? jo["f_price1"].toDouble() : jo["f_price2"].toDouble();
        og.store = __c5config.defaultStore();
        og.total = og.qty *og.price;
        og.discountFactor = fBHistory.value;
        og.discountMode = fBHistory.type;
        og.discountAmount = 0;
        og.canDiscount = g.canDiscount();
        fOGoods.append(og);
        ui->tblData->addEmptyRow();
    }
    countTotal();
    fHttp->httpQueryFinished(sender());
}

void WOrder::readEmarks()
{
    int row = ui->tblData->currentRow();
    if (row < 0) {
        return;
    }
    bool ok = false;
    QString qr = QInputDialog::getText(this, tr("Emark"), tr("Emark"), QLineEdit::Normal, fOGoods[row].emarks, &ok);
    if (!ok) {
        return;
    }
    if (qr.length() > 0 && qr.length() < 20) {
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
    if (s.value("learnaccumulate").toBool()) {
        int row = -1;
        for (int i = 0; i < ui->tblData->rowCount(); i++) {
            if (sender() == ui->tblData->cellWidget(i, col_check_discount)) {
                row = i;
                break;
            }
        }
        if (row > -1) {
            C5Database db(__c5config.dbParams());
            db[":f_candiscount"] = v ? 1 : 0;
            db.update("c_goods", "f_id", fOGoods[row].goods);
            dbgoods->updateField(fOGoods[row].goods, "f_candiscount", v ? 1 : 0);
        }
    }
    countTotal();
}

void WOrder::on_leCode_textChanged(const QString &arg1)
{
    if (arg1 == "+") {
        return;
    }
    if (arg1 == "-") {
        keyMinus();
        return;
    }
    if (arg1 == "*") {
        return;
    }
    if (arg1 == ".") {
        comma();
        return;
    }
}

void WOrder::on_leCode_returnPressed()
{
    //QString code = C5ReplaceCharacter::replace(ui->leCode->text());
    QString code = ui->leCode->text();
    if (code.isEmpty()) {
        return;
    }
    ui->leCode->clear();
    ui->leCode->setFocus();
    if (code.at(0).toLower() == '?' && code.length() > 2 ) {
        if (code.at(1).toLower() == 'c') {
            if (code.at(2).toLower() == 'c') {
                for (int i = 0; i < fOGoods.count(); i++) {
                    OGoods &og = fOGoods[i];
                    og.discountAmount = 0;
                    og.discountFactor = 0;
                    og.discountMode = 0;
                    fBHistory.card = 0;
                    fBHistory.type = 0;
                    fBHistory.value = 0;
                }
                countTotal();
                return;
            } else if (code.at(2).toLower() == 't') {
                int r = ui->tblData->currentRow();
                if (r < 0) {
                    return;
                }
                OGoods &og = fOGoods[r];
                og.discountAmount = 0;
                og.discountFactor = 0;
                og.discountMode = 0;
                countTotal();
                return;
            } else {
                code.remove(0, 2);
                fixCostumer(code);
                return;
            }
        }
    }
    if (code.length() >= 29) {
        if (code.length() == 29) {
            //cigarette
        }
        if (code.length() == 31) {
            //wines
        }
    }
    if (code.at(0).toLower() == '/') {
        code.remove(0, 1);
        discountRow(code);
        return;
    }
    //code.replace(";", "").replace("?", "");
    addGoods(code);
}

void WOrder::on_leCustomerTaxpayerId_returnPressed()
{
    if (ui->leCustomerTaxpayerId->text().isEmpty()) {
        return;
    }
    C5Database db(__c5config.dbParams());
    db[":f_taxcode"] = ui->leCustomerTaxpayerId->text();
    db.exec("select f_id, f_taxname, f_contact from c_partners where f_taxcode=:f_taxcode");
    if (db.nextRow()) {
        fOHeader.partner = db.getInt("f_id");
        ui->leOrganization->setText(db.getString("f_taxname"));
        ui->leContact->setText(db.getString("f_contact"));
        ui->btnF5->setVisible(true);
    } else {
        db[":f_taxcode"] = ui->leCustomerTaxpayerId->text();
        fOHeader.partner = db.insert("c_partners");
    }
}

void WOrder::on_btnSearchPartner_clicked()
{
    DlgSearchPartner d;
    if (d.exec() == QDialog::Accepted) {
        ui->leCustomerTaxpayerId->setText(d.result);
        on_leCustomerTaxpayerId_returnPressed();
    }
}

void WOrder::on_leUseAccumulated_textChanged(const QString &arg1)
{
    if (arg1.toDouble() > ui->leGiftCardAmount->getDouble()) {
        ui->leUseAccumulated->setDouble(ui->leGiftCardAmount->getDouble());
    }
    if (ui->leUseAccumulated->getDouble() > ui->leTotal->getDouble()) {
        ui->leUseAccumulated->setDouble(ui->leTotal->getDouble());
    }
}
