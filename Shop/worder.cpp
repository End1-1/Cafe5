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
#include "c5logsystem.h"
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
#include "odraftsalebody.h"
#include "QRCodeGenerator.h"
#include "c5checkbox.h"
#include <QInputDialog>
#include <QSettings>

static QSettings s(_ORGANIZATION_, _APPLICATION_+ QString("\\") + _MODULE_);

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

    ui->grFlags->setVisible(false);
    fCustomerDisplay = customerDisplay;
    fUser = user;
    fGiftCard = 0;
    fWorking = static_cast<Working*>(parent);
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
}

WOrder::~WOrder()
{
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
            fCustomerDisplay->addRow(g._goodsName, float_str(g.qty, 2), float_str(g.price, 2), float_str(g.total, 2), float_str(g.discountAmount, 2));
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
    clearCode();
    removeRow();
}

void WOrder::keyPlus()
{
    clearCode();
    if (__c5config.getValue(param_shop_deny_qtychange).toInt() == 0) {
        changeQty();
    }
}

void WOrder::keyAsterix()
{
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
    bool successfulEncoding = qrEncode.EncodeData(levelIndex, versionIndex, bExtent, maskIndex, encodeString.toUtf8().data() );
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

bool WOrder::addGoods(int id, double storeqty, double price1, double price2)
{
    if (fOHeader.saleType == -1) {
        if (fOGoods.count() > 0) {
            C5Message::error(tr("Cannot add goods in prepaid mode"));
            return false;
        }
    }
    addGoodsToTable(id, false, storeqty, "", price1, price2);
    return true;
}

int WOrder::addGoodsToTable(int id, bool checkQtyOfStore, double qtyStore, const QString &draftid, double price1, double price2)
{
    DbGoods g(id);
    double price = 0;
    switch (fOHeader.saleType) {
    case SALE_RETAIL:
        price = price1;
        break;
    case SALE_WHOSALE:
        price = price2;
        break;
    default:
        price = price1;
        break;
    }
    ls(QString("%1: %2, %3: %4, %5: %6")
       .arg(tr("New goods with code"))
       .arg(g.scancode())
       .arg(tr("name"))
       .arg(g.goodsName())
       .arg(tr("Price"))
       .arg(price));

    int row = ui->tblData->addEmptyRow();
    auto *ch = new C5CheckBox();
    ch->setCheckable(s.value("learnaccumulate").toBool());
    ch->setChecked(g.canDiscount() == 1);
    connect(ch, &C5CheckBox::clicked, this, &WOrder::checkCardClicked);
    ui->tblData->setCellWidget(row, col_check_discount, ch);
    OGoods og;
    og._groupName = g.group()->groupName();
    og._goodsName = g.goodsName();
    og._unitName = g.unit()->unitName();
    og._barcode = g.scancode();
    og._qtybox = g.qtyBox();
    og.header = fOHeader._id();
    og.goods = id;
    og.taxDept = g.group()->taxDept();
    og.adgCode = g.group()->adgt();
    og.isService = g.isService();
    og.qty = g.unit()->defaultQty();
    og.price = price;
    og.store = __c5config.defaultStore();
    og.total = og.qty * og.price;
    og.discountFactor = fBHistory.value;
    og.discountMode = fBHistory.type;
    og.discountAmount = 0;
    og.canDiscount = g.canDiscount();
    fOGoods.append(og);
    ui->tblData->setCurrentCell(row, 0);
    if (checkQtyOfStore) {
        ui->tblData->item(row, 0)->setData(Qt::UserRole + 100, 1);
    }
    //ui->tblData->setDouble(row, col_stock, (g.unit()->defaultQty()/og._qtybox) + (qtyStore / og._qtybox));
    ui->tblData->setDouble(row, col_stock, (qtyStore / og._qtybox));
    countTotal();

    C5Database db(__c5config.dbParams());
    QString err;
    if (fDraftSale.id.toString().isEmpty()) {
        fDraftSale.id = db.uuid();
        fDraftSale.staff = fUser->id();
        fDraftSale.state = 1;
        fDraftSale.date = QDate::currentDate();
        fDraftSale.time = QTime::currentTime();
        fDraftSale.payment = 1;
        fDraftSale.partner = fOHeader.partner;
        fDraftSale.amount = fOHeader.amountTotal;
        fDraftSale.comment = "";
        fDraftSale.write(db, err);
    }

    if (draftid.isEmpty()) {
        ODraftSaleBody sb;
        sb.id = og.id;
        sb.header = fDraftSale.id.toString();
        sb.state = 1;
        sb.store = __c5config.defaultStore();
        sb.dateAppend = QDate::currentDate();
        sb.timeAppend = QTime::currentTime();
        sb.goods = og.goods;
        sb.qty = og.qty;
        sb.price = og.price;
        sb.write(db, err);
        const_cast<QString&>(draftid) = sb.id.toString();
    }
    ui->tblData->item(row, 0)->setData(Qt::UserRole + 101, draftid);

    ImageLoader *il = new ImageLoader(id, this);
    connect(il, SIGNAL(imageLoaded(QPixmap)), this, SLOT(imageLoaded(QPixmap)));
    connect(il, SIGNAL(noImage()), this, SLOT(noImage()));
    il->start();
    return row;
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
    if (!DlgPaymentChoose::getValues(fOHeader.amountTotal, fOHeader.amountCash, fOHeader.amountCard, fOHeader.amountIdram,
                                     fOHeader.amountTelcell, fOHeader.amountBank, fOHeader.amountCredit,
                                     fOHeader.amountPrepaid, fOHeader.amountDebt,
                                     fOHeader.amountCashIn, fOHeader.amountChange, fOHeader._printFiscal, prepaidReadonly)) {
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
    QJsonObject jh;
    jh["f_id"] = fOHeader._id();
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
    jh["f_comment"] = fOHeader.comment;
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
    if (!fAccCard.isEmpty()) {
        QJsonObject jtemp = fAccCard["card"].toObject();
        jtemp["accumulate"] = ui->leCurrentAccumulated->getDouble();
        fAccCard["card"] = jtemp;
        jdoc["accumulate"] = fAccCard;
    }
    jhistory["f_card"] = fBHistory.card;
    jhistory["f_data"] = fBHistory.data;
    jhistory["f_type"] = fBHistory.type;
    jdoc["history"] = jhistory;

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
                pt.addGoods(g.taxDept.toInt(), //dep
                            g.adgCode, //adg
                            QString::number(g.goods), //goods id
                            g._goodsName, //name
                            g.price / g._qtybox, //price
                            g.qty, //qty
                            fBHistory.value * 100); //discount
            }
            result = pt.makeJsonAndPrint(fOHeader.amountCard + fOHeader.amountIdram + fOHeader.amountTelcell, fOHeader.amountPrepaid, jsonIn, jsonOut, err);
        } else {
            result = pt.printAdvanceJson(fOHeader.amountCash, fOHeader.amountCard, jsonIn, jsonOut, err);
        }


        QJsonParseError jerr;
        QJsonDocument jod = QJsonDocument::fromJson(jsonOut.toUtf8(), &jerr);
        if (jerr.error != QJsonParseError::NoError) {
            err = jerr.errorString();
            jod = QJsonDocument::fromJson(QString("{\"data\":\"" + jsonOut + "\"").toUtf8(), &jerr);
        }


        QJsonObject jtax;
        jtax["f_order"] = fOHeader._id();
        jtax["f_elapsed"] = et.elapsed();
        jtax["f_in"] = QJsonDocument::fromJson(jsonIn.toUtf8()).object();
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

//    bool w = so.write();
//    if (w) {
//        w = so.writeFlags(ui->btnF1->isChecked(), ui->btnF2->isChecked(),
//                          ui->btnF3->isChecked(), ui->btnF4->isChecked(),
//                          ui->btnF5->isChecked());
//    }

    if (fOHeader.partner > 0) {
        OutputOfHeader ooh;
        ooh.make(db, fOHeader._id());
    }

    if (!fDraftSale.id.toString().isEmpty()) {
        db[":f_id"] = fDraftSale.id;
        db.exec("update o_draft_sale set f_state=3 where f_id=:f_id");
    }

    C5LogSystem::writeEvent(QString("%1. %2:%3ms, %4:%5, %6")
                            .arg(tr("Order saved"), tr("Elapsed"), QString::number(t.elapsed()), tr("Order number"), fOHeader.humanId(), fOHeader._id()));
    return true;
}

void WOrder::fixCostumer(const QString &code)
{
    C5Database db(C5Config::replicaDbParams());
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
    C5LogSystem::writeEvent(QString("%1: %2:%3").arg(tr("Fix costumer"), code, db.getString("f_contact")));
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
    double qty = DQty::getQty(tr("Quantity"), this);
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
    int row = ui->tblData->currentRow();
    if (row < 0) {
        return;
    }
    OGoods &g = fOGoods[row];
    if (g.discountFactor > 0.001) {
        C5Message::error(tr("Discount already applied"));
        return;
    }
    C5Database db(C5Config::replicaDbParams());
    db[":f_code"] = code;
    db.exec("select * from b_cards_discount where f_code=:f_code");
    if (!db.nextRow()) {
        return;
    }
    if (QDate::currentDate() > db.getDate("f_dateend")) {
        C5Message::error(tr("Cards was expired"));
        return;
    }
    double v = db.getDouble("f_value");
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
    C5LogSystem::writeEvent(QString("%1: %2:%3, %4:%5").arg(tr("Discount")).arg(tr("Discount type")).arg(discType).arg(tr("Value")).arg(v));
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
    if (__c5config.shopDenyPriceChange() && !(currentPrice <0)) {
        return;
    }
    double price = DQty::getQty(tr("Price"), this);
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
    int code = g.goods;
    QString name = g._goodsName;
    C5Database db(__c5config.dbParams());
    db[":f_id"] = ui->tblData->item(row, 0)->data(Qt::UserRole + 101);
    db[":f_state"] = 2;
    db.exec("update o_draft_sale_body set f_state=:f_state where f_id=:f_id");
    db[":f_store"] = __c5config.defaultStore();
    db[":f_goods"] = code;
    db[":f_qty"] = g.qty / g._qtybox;
    db.exec("update a_store_sale set f_qtyprogram=f_qtyprogram-(:f_qty) where f_store=:f_store and f_goods=:f_goods");
    C5LogSystem::writeEvent(QString("%1 #%2 %3:%4 %5").arg(tr("Remove row")).arg(row).arg(code).arg(name).arg(float_str(g.qty, 2)));
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
        double qty = DQty::getQty(tr("Box"), this);
        if (qty < 0.001) {
            return;
        }
        setQtyOfRow(row, qty * fOGoods.at(row)._qtybox);
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
    if (fAccCard.isEmpty()) {
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
        QPushButton *btn = static_cast<QPushButton*>(ui->tblData->cellWidget(i, col_qr));
        if (btn == nullptr ){
            btn = new QPushButton();
        }
        if (og.emarks.isEmpty()) {
            btn->setIcon(QPixmap(""));
        } else {
            btn->setIcon(QPixmap(":/qr_code.png").scaled(30, 30));
        }
        connect(btn, &QPushButton::clicked, this, &WOrder::readEmarks);
        ui->tblData->setCellWidget(i, col_qr, btn);
        if (ui->tblData->item(i, 0)->data(Qt::UserRole + 100).toInt() == 1) {
            QString err;
            if (!checkQty(og.goods, og.qty / og._qtybox, 0, false, err)) {
                for (int c = 0; c < ui->tblData->columnCount(); c++) {
                    ui->tblData->item(i, c)->setBackgroundColor(Qt::red);
                }
            }
        }

        if (fAccCard.isEmpty() == false) {
            auto *ch = static_cast<C5CheckBox*>(ui->tblData->cellWidget(i, col_check_discount));
            if (ch->isChecked()) {
                accAmount += og.total * fAccCard["card"].toObject()["f_value"].toDouble();
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

bool WOrder::setQtyOfRow(int row, double qty)
{
    OGoods &og = fOGoods[row];
    double oldQty = og.qty;
    DbGoods g(og.goods);
    if (g.acceptIntegerQty()) {
        qty = trunc(qty);
    }
    double diffqty = (qty / og._qtybox) - (og.qty / og._qtybox);
    if (og.isService == 0  && __c5config.getValue(param_shop_dont_check_qty).toInt() == 0) {
        QString err;
        if (!checkQty(g.id(), diffqty, oldQty, true, err)) {
            C5Message::error(og._goodsName + " \r\n" + err);
            return false;
        }
    }
    og.qty = qty;
    C5Database db(__c5config.dbParams());
    db[":f_id"] = ui->tblData->item(row, 0)->data(Qt::UserRole + 101);
    db[":f_qty"] = og.qty / og._qtybox;
    db.exec("update o_draft_sale_body set f_qty=:f_qty where f_id=:f_id");
    C5LogSystem::writeEvent(QString("%1 %2:%3 %4").arg(tr("Change qty"), g.scancode(), g.goodsName(), float_str(qty, 2)));
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

bool WOrder::checkQty(int goods, double qty, double oldqty, bool updateStock, QString &err)
{
    DbGoods gd(goods);
    int storegoods = gd.storeGoods();
    C5Database db(__c5config.dbParams());
    db[":f_store"] = __c5config.defaultStore();
    db[":f_goods"] = storegoods;
    db[":f_qty"] = qty;
    if (!db.exec("select f_qty-f_qtyreserve-f_qtyprogram-(:f_qty) as f_qty from a_store_sale where f_store=:f_store and f_goods=:f_goods ")) {
        err = db.fLastError;
        return false;
    }
    double storeQty = 0;
    if (db.nextRow()) {
        storeQty = db.getDouble(0);
    } else {
        storeQty -= qty;
    }

    if (storeQty < 0) {
        err = tr("Insufficient quantity") + "<br>" + float_str(storeQty, 3);
        return false;
    }

    if (updateStock) {
        db[":f_qty"] = oldqty;
        db[":f_store"] = __c5config.defaultStore();
        db[":f_goods"] = storegoods;
        db.exec("update a_store_sale set f_qtyprogram=f_qtyprogram-(:f_qty) where f_store=:f_store and f_goods=:f_goods");
        db[":f_qty"] = qty + oldqty;
        db[":f_store"] = __c5config.defaultStore();
        db[":f_goods"] = storegoods;
        db.exec("update a_store_sale set f_qtyprogram=f_qtyprogram+(:f_qty) where f_store=:f_store and f_goods=:f_goods");
    }

    return true;
}

void WOrder::imageLoaded(const QPixmap &img)
{
    ui->lbGoodsImage->setPixmap(img.scaled(ui->lbGoodsImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    //ui->wimage->setVisible(true);
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
    QString code = C5ReplaceCharacter::replace(ui->leCode->text());
    if (code.isEmpty()) {
        return;
    }
    C5LogSystem::writeEvent(QString("Code line: %1").arg(code));
    ui->leCode->clear();
    ui->leCode->setFocus();

    if (code.at(0).toLower() == '?' && code.length() > 2 ) {
        if (code.at(1).toLower() == 'c') {
            if (code.at(2) == 'c') {
                for (int i = 0; i < fOGoods.count(); i++) {
                    OGoods &og = fOGoods[i];
                    og.discountAmount = 0;
                    og.discountFactor = 0;
                    og.discountMode = 0;
                    fBHistory.card = 0;
                    fBHistory.type = 0;
                    fBHistory.value = 0;
                    countTotal();
                }
                return;
            } else if (code.at(2) == 't') {
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
    if (code.at(0).toLower() == '/') {
        code.remove(0, 1);
        discountRow(code);
        return;
    }

    C5Database db(__c5config.replicaDbParams());
    code.replace(";", "").replace("?", "");

    QJsonObject checkCard;
    checkCard["code"] = code;
    QString sql = QString("select sf_check_card('%1')").arg(__jsonstr(checkCard));
    db.exec(sql);
    db.nextRow();
    checkCard = __strjson(db.getString(0));
    if (checkCard["status"].toInt() == 1) {
        checkCard = checkCard["data"].toObject();
        ui->leDisc->setText(QString("%1%").arg(float_str(checkCard["card"].toObject()["f_value"].toDouble() * 100, 2)));
        ui->leDisc->setVisible(true);
        ui->lbDisc->setVisible(true);
        ui->leGiftCardAmount->setDouble(checkCard["history"].toObject()["f_amount"].toDouble());
        ui->leContact->setText(checkCard["customer"].toObject()["f_name"].toString());
        fAccCard = checkCard;
        ui->leUseAccumulated->setReadOnly(!(ui->leGiftCardAmount->getDouble() > 0));
        ui->lbUseAccumulated->setVisible(true);
        ui->leUseAccumulated->setVisible(true);
        countTotal();
        return;
    }


    QJsonObject params;
    params["scancode"] = code;
    params["settings"] = __c5config.fSettingsName;
    params["updatestore"] = 1;
    params["store"] = __c5config.defaultStore();
    db.exec(QString("select sf_shop_add_goods('%1')").arg(QString(QJsonDocument(params).toJson(QJsonDocument::Compact))));
    if (!db.nextRow()) {
        C5Message::error("Call sf_shop_add_goods failed");
        return;
    }
    params = QJsonDocument::fromJson(db.getString(0).toUtf8()).object();
    if (params["status"].toInt() == 0) {
        C5Message::error(params["data"].toString());
        return;
    }
    params = params["data"].toObject();
    if (params["type"].toInt() == 2) {
        fGiftCard = params["f_id"].toInt();
        ui->lbGiftCard->setText(params["f_code"].toString().right(4));
        ui->leGiftCardAmount->setDouble(params["f_amount"].toString().toDouble());
        return;
    }

//    db[":f_code"] = code;
//    db.exec("select c.f_id, c.f_code, sum(h.f_amount) as f_amount "
//            "from b_gift_card_history h "
//            "inner join b_gift_card c on c.f_id=h.f_card "
//            "where c.f_code=:f_code ");
//    if (db.nextRow()) {
//        fGiftCard = db.getInt("f_id");
//        if (fGiftCard > 0) {
//            ui->lbGiftCard->setText(db.getString("f_code").right(4));
//            ui->leGiftCardAmount->setDouble(db.getDouble("f_amount"));
//            return;
//        }
//    }

//  TODO: FOR FUTURE: ADD SCALE SCANCODE SUPPORT
//    QStringList scalecode = __c5config.getValue(param_shop_scalecode).split(",");
//    for (const QString &s: scalecode) {
//        if (s.contains(code.mid(0, 2))) {
//            if (code.length() != 13) {
//                int id = dbgoods->idOfScancode(code);
//                if (id == 0) {
//                    if (fWorking->fMultiscancode.contains(code)) {
//                        QString mcode = fWorking->fMultiscancode[code];
//                        id = dbgoods->idOfScancode(mcode);
//                    }
//                }
//                if (id == 0) {
//                    ls(tr("Invalid code entered: ") + code);
//                    return;
//                }
//                addGoods(id);
//                return;
//            }
//            QString code2 = QString("%1").arg(code.midRef(2, 5).toInt());
//            QString qtyStr = code.mid(7,5);
//            int id2 = dbgoods->idOfScancode(code2);
//            if (id2 > 0) {
//                addGoods(id2);
//            } else {
//                ls(tr("Invalid code entered: ") + code + "/" + code2);
//                return;
//            }

//            int row = lastRow();
//            if (row < 0) {
//                return;
//            }
//            setQtyOfRow(row, qtyStr.toDouble() / 1000);
//            return;
//        }
//    }


    addGoods(params["f_id"].toInt(), params["storeqty"].toDouble(), params["price1"].toDouble(), params["price2"].toDouble());

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

