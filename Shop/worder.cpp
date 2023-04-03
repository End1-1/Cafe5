#include "worder.h"
#include "ui_worder.h"
#include "c5database.h"
#include "c5config.h"
#include "printtaxn.h"
#include "imageloader.h"
#include "dqty.h"
#include "c5message.h"
#include "c5permissions.h"
#include "c5printing.h"
#include "preorders.h"
#include "c5logsystem.h"
#include "printreceipt.h"
#include "selectstaff.h"
#include "c5shoporder.h"
#include "dlgsearchpartner.h"
#include "working.h"
#include "dlgpaymentchoose.h"
#include "c5user.h"
#include "datadriver.h"
#include "wcustomerdisplay.h"
#include "c5utils.h"
#include "odraftsalebody.h"
#include "c5printrecipta4.h"
#include "c5storedraftwriter.h"
#include "c5replication.h"
#include "QRCodeGenerator.h"
#include "threadcheckmessage.h"
#include <QInputDialog>

#define col_bacode 0
#define col_name 1
#define col_qty 2
#define col_unit 3
#define col_price 4
#define col_total 5
#define col_discamount 6
#define col_discmode 7
#define col_discvalue 8
#define col_stock 9

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

    ui->tblData->setColumnWidth(col_bacode, 120);
    ui->tblData->setColumnWidth(col_name, 300);
    ui->tblData->setColumnWidth(col_qty, 100);
    ui->tblData->setColumnWidth(col_unit, 100);
    ui->tblData->setColumnWidth(col_price, 100);
    ui->tblData->setColumnWidth(col_total, 100);
    ui->tblData->setColumnWidth(col_discamount, 100);
    ui->tblData->setColumnWidth(col_discmode, 200);
    ui->tblData->setColumnWidth(col_discvalue, 120);
    ui->tblData->setColumnWidth(col_stock, 100);
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

bool WOrder::addGoods(int id)
{
    if (fOHeader.saleType == -1) {
        if (fOGoods.count() > 0) {
            C5Message::error(tr("Cannot add goods in prepaid mode"));
            return false;
        }
    }
    DbGoods g(id);
    if (!g.isService() && __c5config.getValue(param_shop_dont_check_qty).toInt() == 0) {
        QString err;
        if (!checkQty(id, g.unit()->defaultQty(), true, err)) {
            C5Database db(__c5config.dbParams());
            db[":f_id"] = id;
            db.exec("select f_name from c_goods where f_id=:f_id");
            db.nextRow();
            C5Message::error(db.getString(0) + "\r\n" + err);
            return false;
        }
    }
    addGoodsToTable(id, false, "");
    return true;
}

int WOrder::addGoodsToTable(int id, bool checkQtyOfStore, const QString &draftid)
{
    C5Database db(C5Config::dbParams());
    db[":f_goods"] = id;
    db[":f_currency"] = C5Config::getValue(param_default_currency).toInt();
    db.exec("select f_price1, f_price2 from c_goods_prices where f_goods=:f_goods and f_currency=:f_currency");
    if (db.nextRow() == false) {
        C5Message::error(tr("Goods prices undefined"));
        return -1;
    }


    DbGoods g(id);
    double price = 0;
    switch (fOHeader.saleType) {
    case SALE_RETAIL:
        price = db.getDouble("f_price1");
        break;
    case SALE_WHOSALE:
        price = db.getDouble("f_price2");
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
    OGoods og;
    og._goodsName = g.goodsName();
    og._unitName = g.unit()->unitName();
    og._barcode = g.scancode();
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
    fOGoods.append(og);
    ui->tblData->setCurrentCell(row, 0);
    if (checkQtyOfStore) {
        ui->tblData->item(row, 0)->setData(Qt::UserRole + 100, 1);
    }
    db[":f_goods"] = og.goods;
    db[":f_store"] = __c5config.defaultStore();
    db.exec("select f_qty-f_qtyreserve-f_qtyprogram from a_store_sale where f_goods=:f_goods and f_store=:f_store");
    if (db.nextRow()) {
        ui->tblData->setDouble(row, col_stock, db.getDouble(0) + 1);
    }
    countTotal();

    QString err;
    if (fDraftSale.id.isEmpty()) {
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
        sb.header = fDraftSale.id;
        sb.state = 1;
        sb.store = __c5config.defaultStore();
        sb.dateAppend = QDate::currentDate();
        sb.timeAppend = QTime::currentTime();
        sb.goods = og.goods;
        sb.qty = og.qty;
        sb.price = og.price;
        sb.write(db, err);
        const_cast<QString&>(draftid) = sb.id;
    }
    ui->tblData->item(row, 0)->setData(Qt::UserRole + 101, draftid);

    ImageLoader *il = new ImageLoader(g.scancode(), this);
    connect(il, SIGNAL(imageLoaded(QPixmap)), this, SLOT(imageLoaded(QPixmap)));
    connect(il, SIGNAL(noImage()), this, SLOT(noImage()));
    il->start();
    return row;
}

bool WOrder::writeOrder()
{
    if (fOHeader.amountCash < 0.001
            && fOHeader.amountCard < 0.001
            && fOHeader.amountBank < 0.001
            && fOHeader.amountIdram < 0.001
            && fOHeader.amountTelcell < 0.001
            && fOHeader.amountPayX < 0.001
            && fOHeader.amountPrepaid < 0.001) {
        fOHeader.amountCash = fOHeader.amountTotal;
    }
    if (!DlgPaymentChoose::getValues(fOHeader.amountTotal, fOHeader.amountCash, fOHeader.amountCard, fOHeader.amountIdram,
                                     fOHeader.amountTelcell, fOHeader.amountBank, fOHeader.amountPrepaid, fOHeader.amountDebt,
                                     fOHeader.amountCashIn, fOHeader.amountChange, fOHeader._printFiscal)) {
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
        do {
            if (ss.exec() == QDialog::Rejected) {
                break;
            }
            u = ss.fUser;
        } while (!u->isValid());
    }
    C5ShopOrder so(fOHeader, fBHistory, fOGoods);
    so.fWriteAdvance = fOHeader.saleType == -1;
    bool w = so.write();
    if (w) {
        w = so.writeFlags(ui->btnF1->isChecked(), ui->btnF2->isChecked(),
                          ui->btnF3->isChecked(), ui->btnF4->isChecked(),
                          ui->btnF5->isChecked());
    }
    C5Database db(__c5config.dbParams());
    if (w) {
        foreach (const OGoods &g, fOGoods) {
            if (g.isService) {
                continue;
            }
            db[":f_store"] = __c5config.defaultStore();
            db[":f_goods"] = g.goods;
            db[":f_qty"] = g.qty;
            db.exec("update a_store_sale set f_qty=f_qty-:f_qty, f_qtyprogram=f_qtyprogram-:f_qty where f_store=:f_store and f_goods=:f_goods");
        }
        if (!fPreorderUUID.isEmpty()) {
            db[":f_state"] = 2;
            db.update("op_header", "f_id", fPreorderUUID);
        }
    }
    if (w && fOHeader.saleType == -1) {
        C5Database dbr(__c5config.replicaDbParams());
        dbr[":f_id"] = fOGoods.at(0).goods;
        dbr.exec("select f_scancode from c_goods where f_id=:f_id");
        if (dbr.nextRow()) {
            QString cardcode = "2211" +  dbr.getString("f_scancode");
            dbr[":f_code"] = cardcode;
            dbr.exec("select * from b_gift_card where f_code=:f_code");
            if (dbr.nextRow()) {
                int giftid = dbr.getInt("f_id");
                dbr[":f_code"] = cardcode;
                dbr[":f_datesaled"] = QDate::currentDate();
                dbr.exec("update b_gift_card set f_datesaled=:f_datesaled where f_code=:f_code");
                dbr[":f_card"] = giftid;
                dbr[":f_trsale"] = fOHeader.id;
                dbr[":f_amount"] = ui->leTotal->getDouble();
                dbr.insert("b_gift_card_history");
            }
        }
    }
    if (w && fGiftCard > 0) {
        C5Database dbr(__c5config.replicaDbParams());
        if (fOHeader.amountPrepaid > 0.001) {
            dbr[":f_card"] = fGiftCard;
            dbr[":f_trsale"] = fOHeader.id;
            dbr[":f_amount"] = -1 * fOHeader.amountPrepaid;
            dbr.insert("b_gift_card_history");
        } else {
            dbr[":f_id"] = fGiftCard;
            dbr.exec("select f_code from b_gift_card where f_id=:f_id");
            dbr.nextRow();
            dbr[":f_code"] = dbr.getString("f_code");
            dbr.exec("select f_value from b_cards_discount where f_code=:f_code");
            dbr.nextRow();
            dbr[":f_card"] = fGiftCard;
            dbr[":f_trsale"] = fOHeader.id;
            dbr[":f_amount"] = ui->leTotal->getDouble() * dbr.getDouble("f_value");
            dbr.insert("b_gift_card_history");
        }
    }

    if (!fDraftSale.id.isEmpty()) {
        db[":f_id"] = fDraftSale.id;
        db.exec("update o_draft_sale set f_state=3 where f_id=:f_id");
    }

//    if (C5Message::question(tr("Print?")) == QDialog::Accepted) {
//        C5PrintReciptA4 p(__c5config.dbParams(), fOrderUUID, this);
//        QString err;
//        if (p.print(err)) {

//        }
//    }

    C5LogSystem::writeEvent(QString("%1. %2:%3ms, %4:%5, %6")
                            .arg(tr("Order saved"), tr("Elapsed"), QString::number(t.elapsed()), tr("Order number"), fOHeader.humanId(), fOHeader._id()));
    return w;
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
    QString code = g.goods;
    QString name = g._goodsName;
    C5Database db(__c5config.dbParams());
    db[":f_id"] = ui->tblData->item(row, 0)->data(Qt::UserRole + 1);
    db[":f_state"] = 2;
    db.exec("update o_draft_sale_body set f_state=:f_state where f_id=:f_id");
    db[":f_store"] = __c5config.defaultStore();
    db[":f_goods"] = code.toInt();
    db[":f_qty"] = g.qty;
    db.exec("update a_store_sale set f_qtyprogram=f_qtyprogram-:f_qty where f_store=:f_store and f_goods=:f_goods");
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
    fOHeader.countAmount(fOGoods, fBHistory);
    ui->leTotal->setDouble(fOHeader.amountTotal);
    ui->leDisc->setDouble(fOHeader.amountDiscount);
    for (int i = 0; i < fOGoods.count(); i++) {
        const OGoods &og = fOGoods.at(i);
        ui->tblData->setData(i, col_bacode, og._barcode);
        ui->tblData->setData(i, col_name, og._goodsName);
        ui->tblData->setData(i, col_qty, og.qty);
        ui->tblData->setData(i, col_price, og.price);
        ui->tblData->setData(i, col_unit, og._unitName);
        ui->tblData->setData(i, col_total, og.total);
        ui->tblData->setData(i, col_discamount, og.discountAmount);
        ui->tblData->setData(i, col_discvalue, og.discountFactor);
        ui->tblData->setData(i, col_discmode, og.discountMode);
        if (ui->tblData->item(i, 0)->data(Qt::UserRole + 100).toInt() == 1) {
            QString err;
            if (!checkQty(og.goods, og.qty, false, err)) {
                for (int c = 0; c < ui->tblData->columnCount(); c++) {
                    ui->tblData->item(i, c)->setBackgroundColor(Qt::red);
                }
            }
        }
    }
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
    DbGoods g(og.goods);
    if (g.acceptIntegerQty()) {
        qty = trunc(qty);
    }
    double diffqty = qty - og.qty;
    if (og.isService == 0  && __c5config.getValue(param_shop_dont_check_qty).toInt() == 0) {
        QString err;
        if (!checkQty(g.id(), diffqty, true, err)) {
            C5Message::error(og._goodsName + " \r\n" + err);
            return false;
        }
    }
    og.qty = qty;
    C5Database db(__c5config.dbParams());
    db[":f_id"] = ui->tblData->item(row, 0)->data(Qt::UserRole + 101);
    db[":f_qty"] = og.qty;
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

bool WOrder::checkQty(int goods, double qty, bool updateStock, QString &err)
{
    DbGoods gd(goods);
    int storegoods = gd.storeGoods();
    C5Database db(__c5config.dbParams());
    db[":f_store"] = __c5config.defaultStore();
    db[":f_goods"] = storegoods;
    db[":f_qty"] = qty;
    if (!db.exec("select f_qty-f_qtyreserve-f_qtyprogram-:f_qty as f_qty from a_store_sale where f_store=:f_store and f_goods=:f_goods ")) {
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
        db[":f_qty"] = qty;
        db[":f_store"] = __c5config.defaultStore();
        db[":f_goods"] = storegoods;
        db.exec("update a_store_sale set f_qtyprogram=f_qtyprogram+:f_qty where f_store=:f_store and f_goods=:f_goods");
    }

    return true;
}

void WOrder::imageLoaded(const QPixmap &img)
{
    ui->lbGoodsImage->setPixmap(img.scaled(ui->lbGoodsImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    //ui->wimage->setVisible(true);
}

void WOrder::noImage()
{
    //ui->wimage->setVisible(false);
}

void WOrder::on_btnRemove_clicked()
{
    if (fPreorderUUID.isEmpty()) {
        C5Message::error(tr("Cannot remove preorder that wasnt saved"));
        return;
    }
    if (C5Message::question(tr("Confirm to remove")) != QDialog::Accepted) {
        return;
    }
    C5Database db(__c5config.replicaDbParams());
    db[":f_state"] = 3;
    db.update("op_header", "f_id", fPreorderUUID);
    fWorking->fTab->currentWidget()->deleteLater();
    fWorking->fTab->removeTab(fWorking->fTab->currentIndex());
}

void WOrder::on_leCode_textChanged(const QString &arg1)
{
    if (arg1 == "+") {
        return;
    }
    if (arg1 == "-") {
        return;
    }
    if (arg1 == "*") {
        return;
    }
}

void WOrder::on_leCode_returnPressed()
{
    QString code = ui->leCode->text();
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
    db[":f_code"] = code;
    db.exec("select c.f_id, c.f_code, sum(h.f_amount) as f_amount "
            "from b_gift_card_history h "
            "inner join b_gift_card c on c.f_id=h.f_card "
            "where c.f_code=:f_code ");
    if (db.nextRow()) {
        fGiftCard = db.getInt("f_id");
        if (fGiftCard > 0) {
            ui->lbGiftCard->setText(db.getString("f_code").right(4));
            ui->leGiftCardAmount->setDouble(db.getDouble("f_amount"));
            return;
        }
    }

    QStringList scalecode = __c5config.getValue(param_shop_scalecode).split(",");
    for (const QString &s: scalecode) {
        if (s.contains(code.mid(0, 2))) {
            if (code.length() != 13) {
                int id = dbgoods->idOfScancode(code);
                if (id == 0) {
                    if (fWorking->fMultiscancode.contains(code)) {
                        QString mcode = fWorking->fMultiscancode[code];
                        id = dbgoods->idOfScancode(mcode);
                    }
                }
                if (id == 0) {
                    ls(tr("Invalid code entered: ") + code);
                    return;
                }
                addGoods(id);
                return;
            }
            QString code2 = QString("%1").arg(code.midRef(2, 5).toInt());
            QString qtyStr = code.mid(7,5);
            int id2 = dbgoods->idOfScancode(code2);
            if (id2 > 0) {
                addGoods(id2);
            } else {
                ls(tr("Invalid code entered: ") + code + "/" + code2);
                return;
            }

            int row = lastRow();
            if (row < 0) {
                return;
            }
            setQtyOfRow(row, qtyStr.toDouble() / 1000);
            return;
        }
    }

    int id = dbgoods->idOfScancode(code);
    if (id == 0) {
        if (fWorking->fMultiscancode.contains(code)) {
            QString mcode = fWorking->fMultiscancode[code];
            id = dbgoods->idOfScancode(mcode);
        }
    }
    if (id == 0) {
        ls(tr("Invalid code entered: ") + code);
        return;
    }
    addGoods(id);

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
