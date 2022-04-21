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
#include "working.h"
#include "c5user.h"
#include "datadriver.h"
#include "c5utils.h"
#include "c5storedraftwriter.h"
#include "c5replication.h"
#include "threadcheckmessage.h"
#include <QInputDialog>

#define col_qty 2
#define col_price 4
#define col_total 5
#define col_discount_value 11
#define col_discount_mode 12

WOrder::WOrder(C5User *user, int saleType, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WOrder)
{
    ui->setupUi(this);
    ui->lbAdvance->setVisible(false);
    ui->leAdvance->setVisible(false);
    ui->grFlags->setVisible(false);
    ui->tblGoods->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    fSaleType = saleType;
    fPartner = 0;
    fUser = user;
    fWorking = static_cast<Working*>(parent);
    fDateOpen = QDate::currentDate();
    fTimeOpen = QTime::currentTime();
    ui->tblGoods->setColumnWidths(ui->tblGoods->columnCount(), 0, 500, 80, 80, 120, 120, 0, 0, 0, 0, 0, 100, 00);
    fCostumerId = 0;
    fCardValue = 0;
    fCardMode = 0;
    ui->lbDisc->setVisible(false);
    ui->leDisc->setVisible(false);
    ui->leCard->setValidator(new QDoubleValidator(0, 1000000000, 2));
    ui->leCash->setValidator(new QDoubleValidator(0, 1000000000, 2));
    ui->leChange->setValidator(new QDoubleValidator(0, 1000000000, 2));
    noImage();
    connect(ui->leCard, &C5LineEdit::focusOut, [this](){
        C5LogSystem::writeEvent(QString("%1: %2").arg(tr("Card value"), ui->leCard->text()));
    });
    connect(ui->leCash, &C5LineEdit::focusOut, [this](){
        C5LogSystem::writeEvent(QString("%1: %2").arg(tr("Cash value"), ui->leCard->text()));
    });
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
}

WOrder::~WOrder()
{
    delete ui;
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

void WOrder::focusCash()
{
    ui->leCash->setFocus();
    ui->leCash->setSelection(0, 1000);
}

void WOrder::focusCard()
{
    ui->leCard->setFocus();
    ui->leCard->setSelection(0, 1000);
}

void WOrder::focusTaxpayerId()
{
    ui->leCustomerTaxpayerId->setFocus();
    ui->leCustomerTaxpayerId->setSelection(0, 1000);
}

void WOrder::addGoods(int id)
{
    DbGoods g(id);
    if (!g.isService() && __c5config.getValue(param_shop_dont_check_qty).toInt() == 0) {
        QString err;
        if (!checkQty(id, g.unit()->defaultQty(), err)) {
            C5Message::error(err);
            return;
        }
    }

    addGoodsToTable(id);
}

void WOrder::addGoodsToTable(int id)
{
    DbGoods g(id);
    double price = 0;
    switch (fSaleType) {
    case SALE_RETAIL:
    case SALE_PREORDER:
        price = g.retailPrice();
        break;
    case SALE_WHOSALE:
        price = g.whosalePrice();
        break;
    }
    ls(QString("%1: %2, %3: %4, %5: %6")
       .arg(tr("New goods with code"))
       .arg(g.scancode())
       .arg(tr("name"))
       .arg(g.goodsName())
       .arg(tr("Price"))
       .arg(price));
    int row = ui->tblGoods->addEmptyRow();
    ui->tblGoods->item(row, 0)->setData(Qt::UserRole, QVariant::fromValue(id));
    ui->tblGoods->setInteger(row, 0, id);
    //ui->tblGoods->setString(row, 1, g.goodsName() + " " + g.scancode());
    ui->tblGoods->setString(row, 1, g.goodsName());
    ui->tblGoods->setDouble(row, 2, g.unit()->defaultQty());
    ui->tblGoods->item(row, 2)->fDecimals = 3;
    ui->tblGoods->setString(row, 3, g.unit()->unitName());
    ui->tblGoods->setDouble(row, 4, price);
    ui->tblGoods->setDouble(row, 5, g.unit()->defaultQty() * price);
    ui->tblGoods->setString(row, 6, g.group()->taxDept());
    ui->tblGoods->setString(row, 7, g.group()->adgt());
    ui->tblGoods->setString(row, 8, "");
    ui->tblGoods->setInteger(row, 9, g.unit()->id());
    ui->tblGoods->setInteger(row, 10, g.isService() ? 1 : 0);
    ui->tblGoods->setDouble(row, col_discount_value, 0);
    ui->tblGoods->setString(row, col_discount_mode, "");
    ui->tblGoods->setCurrentItem(ui->tblGoods->item(row, 0));
    countTotal();

    C5Database db(__c5config.dbParams());
    db[":f_station"] = hostinfo + ": " + hostusername();
    db[":f_saletype"] = fSaleType;
    db[":f_window"] = fWorking->fTab->currentIndex();
    db[":f_row"] = row;
    db[":f_state"] = 0;
    db[":f_goodsid"] = g.id();
    db[":f_name"] = g.goodsName() + " " + g.scancode();
    db[":f_qty"] = g.unit()->defaultQty();
    db[":f_unit"] = g.unit()->unitName();
    db[":f_price"] = price;
    db[":f_total"] = g.unit()->defaultQty() * price;
    db[":f_taxdept"] = g.group()->taxDept();
    db[":f_adgcode"] = g.group()->adgt();
    db[":f_tablerec"] = 0;
    db[":f_unitcode"] = g.unit()->id();
    db[":f_service"] = g.isService();
    db[":f_discountvalue"] = 0;
    db[":f_discountmode"] = "";
    db.insert("a_sale_temp");

    ImageLoader *il = new ImageLoader(g.scancode(), this);
    connect(il, SIGNAL(imageLoaded(QPixmap)), this, SLOT(imageLoaded(QPixmap)));
    connect(il, SIGNAL(noImage()), this, SLOT(noImage()));
    il->start();
}

bool WOrder::writeOrder()
{
    QElapsedTimer t;
    t.start();

    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if (ui->tblGoods->getDouble(i, 2) < 0.0001) {
            C5Message::error(tr("Invalid qty"));
            return false;
        }
        if (ui->tblGoods->getDouble(i, 4) < 0) {
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
    if (fSaleType == SALE_PREORDER) {
        if (fPreorderUUID.isEmpty()) {
            C5Message::error(tr("Save first or make your sale as usual"));
            return false;
        }
    }
    QList<IGoods> goods;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        IGoods g;
        DbGoods gt(ui->tblGoods->getInteger(i, 0));
        g.taxDept = ui->tblGoods->getString(i, 6);
        g.taxAdg = ui->tblGoods->getString(i, 7);
        g.goodsId = ui->tblGoods->getString(i, 0).toInt();
        g.goodsName = ui->tblGoods->getString(i, 1);
        g.goodsPrice = ui->tblGoods->getDouble(i, 4);
        g.goodsQty = ui->tblGoods->getDouble(i, 2);
        g.goodsTotal = ui->tblGoods->getDouble(i, 5);
        g.unitId = ui->tblGoods->getInteger(i, 9);
        g.isService = ui->tblGoods->getInteger(i, 10) == 1;
        g.discountFactor = ui->tblGoods->item(i, col_discount_value)->data(Qt::UserRole).toDouble();
        g.discountMode = ui->tblGoods->item(i, col_discount_mode)->data(Qt::UserRole).toInt();
        g.storeId = __c5config.defaultStore();
        g.writeStoreDocBeforeOutput = gt.writeStoreInputBeforeSale();
        g.lastInputPrice = gt.lastInputPrice();
        goods.append(g);
    }
    C5ShopOrder so(u);
    so.setPayment(ui->leCash->getDouble(), ui->leChange->getDouble());
    so.setPartner(fPartner, ui->lePartner->text());
    so.setDiscount(fCostumerId, fCardId, fCardMode, fCardValue);
    so.setParams(fDateOpen, fTimeOpen, fSaleType);
    C5LogSystem::writeEvent(QString("%1. %2:%3, %4:%5, %6:%7, %8:%9").arg(tr("Before write"), tr("Total"), ui->leTotal->text(), tr("Card"), ui->leCard->text(),tr("Advance"), 0, tr("Discount"), ui->leDisc->text()));
    bool w = so.write(ui->leTotal->getDouble(), ui->leCard->getDouble(), 0, ui->leDisc->getDouble(), false, goods, fCardValue, fCardMode);
    if (w) {
        w = so.writeFlags(ui->btnF1->isChecked(), ui->btnF2->isChecked(), ui->btnF3->isChecked(), ui->btnF4->isChecked(), ui->btnF5->isChecked());
    }
    fOrderUUID = so.fHeader;
    C5Database db(__c5config.dbParams());
    if (w) {
        foreach (const IGoods &g, goods) {
            if (g.isService) {
                continue;
            }
            db[":f_store"] = __c5config.defaultStore();
            db[":f_goods"] = g.goodsId;
            db[":f_qty"] = g.goodsQty;
            db.exec("update a_store_sale set f_qty=f_qty-:f_qty, f_qtyprogram=f_qtyprogram-:f_qty where f_store=:f_store and f_goods=:f_goods");
        }
        if (!fPreorderUUID.isEmpty()) {
            db[":f_state"] = 2;
            db.update("op_header", "f_id", fPreorderUUID);
        }
    }
    if (w) {
        if (ui->chDebt->isChecked()) {
            if (fPartner > 0) {
                db[":f_costumer"] = fPartner;
                db[":f_order"] = so.fHeader;
                db[":f_amount"] = ui->leTotal->getDouble() * -1;
                db[":f_date"] = QDate::currentDate();
                db[":f_govnumber"] = "";
                db.insert("b_clients_debts", false);
            }
        }

        db[":f_station"] = hostinfo + ": " + hostusername();
        db[":f_order"] = so.fHeader;
        db[":f_window"] = fWorking->fTab->currentIndex();
        db.exec("update a_sale_temp set f_state=2, f_order=:f_order where f_station=:f_station and f_window=:f_window and f_state=0");
    }
    C5LogSystem::writeEvent(QString("%1. %2:%3ms, %4:%5, %6").arg(tr("Order saved"), tr("Elapsed"), QString::number(t.elapsed()), tr("Order number"), so.fHallId, so.fHeader));
    return w;
}

//bool WOrder::writePreorder()
//{
//    if (ui->tblGoods->rowCount() == 0) {
//        C5Message::error(tr("Empty order"));
//        return false;
//    }
//    if (ui->leContactName->text().isEmpty()) {
//        C5Message::error(tr("Customer name cannot be empty"));
//        return false;
//    }
//    bool r = true;
//    bool u = true;
//    C5Database db(__c5config.dbParams());
//    db.startTransaction();
//    db[":f_contact"] = ui->leContactName->text();
//    db.exec("select * from c_partners where f_contact=:f_contact and f_phone=:f_phone");
//    if (db.nextRow()) {
//        fPartner = db.getInt("f_id");
//    } else {
//        db[":f_contact"] = ui->leContactName->text();
//        db[":f_phone"] = ui->leContactPhone->text();
//        fPartner = db.insert("c_partners");
//    }
//    if (fPreorderUUID.isEmpty()) {
//        u = false;
//        fPreorderUUID = db.uuid_bin();
//    }
//    db[":f_id"] = fPreorderUUID;
//    db[":f_state"] = 1;
//    db[":f_datecreate"] = QDate::currentDate();
//    db[":f_timecreate"] = QTime::currentTime();
//    db[":f_staff"] = fUser->id();
//    db[":f_datefor"] = ui->dePreorerDate->date();
//    db[":f_timefor"] = ui->tmTime->time();
//    db[":f_total"] = ui->leTotal->getDouble();
//    db[":f_advance"] = ui->leAdvance->getDouble();
//    db[":f_customer"] = fPartner;
//    db[":f_info"] = ui->tePreorderInfo->toPlainText();
//    if (u) {
//        r = db.update("op_header", "f_id", fPreorderUUID);
//    } else {
//        r = db.insert("op_header", false);
//    }
//    if (!r) {
//        db.rollback();
//        C5Message::error(db.fLastError);
//        return false;
//    }
//    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
//        u = true;
//        QByteArray bid = ui->tblGoods->getData(i, 8).toByteArray();
//        if (bid.isEmpty()) {
//            u = false;
//            bid = db.uuid_bin();
//        }
//        db[":f_id"] = bid;
//        db[":f_header"] = fPreorderUUID;
//        db[":f_goods"] = ui->tblGoods->getInteger(i, 0);
//        db[":f_qty"] = ui->tblGoods->getDouble(i, 2);
//        db[":f_price"] = ui->tblGoods->getDouble(i, 4);
//        if (u) {
//            r = db.update("op_body", "f_id", bid);
//        } else {
//            r = db.insert("op_body", false);
//        }
//        if (!r) {
//            db.rollback();
//            C5Message::error(db.fLastError);
//            return false;
//        }
//    }
//    db.commit();
//    C5Message::info(tr("Saved"));
//    return r;
//}

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
    fCostumerId = db.getInt("f_client");
    fCardId = db.getInt("f_id");
    fCardMode = db.getInt("f_mode");
    fCardValue = db.getDouble("f_value");
    db[":f_id"] = fCostumerId;
    db.exec("select * from c_partners where f_id=:f_id");
    if (!db.nextRow()) {
        return;
    }
    if (fCardValue < 0) {
        double v;
        if (!getDiscountValue(fCardMode, v)) {
            fCardId = 0;
            fCostumerId = 0;
            fCardMode = 0;
            fCardValue = 0;
            return;
        }
        if (fCardMode == CARD_TYPE_DISCOUNT) {
            fCardValue = v / 100;
        } else {
            fCardValue = v;
        }
    }
    ui->leDisc->setVisible(true);
    ui->leCustomer->setVisible(true);
    ui->lbCustomer->setVisible(true);
    ui->lbDisc->setVisible(true);
    ui->lbDept->setVisible(true);
    ui->chDebt->setVisible(true);
    ui->leCustomer->setText(db.getString("f_firstname") + " " + db.getString("f_lastname"));
    C5LogSystem::writeEvent(QString("%1: %2:%3").arg(tr("Fix costumer")).arg(code).arg(db.getString("f_firstname") + " " + db.getString("f_lastname")));
    countTotal();
}

void WOrder::changeQty()
{
    int row = ui->tblGoods->currentRow();
    if (row < 0) {
        return;
    }
    if (ui->tblGoods->item(row, col_discount_mode)->data(Qt::UserRole).toInt() == CARD_TYPE_MANUAL) {
        C5Message::error(tr("Cannot change the quantity on selected row with manual discount mode"));
        return;
    }
    double qty = DQty::getQty(tr("Quantity"), this);
    if (qty < 0.001) {
        return;
    }
    setQtyOfRow(row, qty);
}

void WOrder::discountRow(const QString &code)
{
    int row = ui->tblGoods->currentRow();
    if (row < 0) {
        return;
    }
    if (ui->tblGoods->getDouble(row, 11) > 0.001) {
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

    switch (discType) {
    case CARD_TYPE_DISCOUNT:
        ui->tblGoods->setDouble(row, col_discount_value, v * 100);
        ui->tblGoods->setString(row, col_discount_mode, "%");
        ui->tblGoods->setDouble(row, col_price, ui->tblGoods->getDouble(row, col_price) - (ui->tblGoods->getDouble(row, col_price) * v));
        ui->tblGoods->setDouble(row, col_total, ui->tblGoods->getDouble(row, col_qty) * ui->tblGoods->getDouble(row, col_price));
        break;
    case CARD_TYPE_MANUAL:
        if (v > ui->tblGoods->getDouble(row, col_total)) {
            C5Message::error(tr("Discount amount greater than total amount"));
            return;
        }
        ui->tblGoods->setDouble(row, col_total, ui->tblGoods->getDouble(row, col_total) - v);
        ui->tblGoods->setDouble(row, col_price, ui->tblGoods->getDouble(row, col_total) / ui->tblGoods->getDouble(row, col_qty));
        ui->tblGoods->setDouble(row, col_discount_value, v);
        ui->tblGoods->setString(row, col_discount_mode, "$");
        break;
    default:
        C5Message::error(tr("This discount mode is not supported"));
        return;
    }
    ui->tblGoods->item(row, col_discount_value)->setData(Qt::UserRole, v);
    ui->tblGoods->item(row, col_discount_mode)->setData(Qt::UserRole, discType);
    C5LogSystem::writeEvent(QString("%1: %2:%3, %4:%5").arg(tr("Discount")).arg(tr("Discount type")).arg(discType).arg(tr("Value")).arg(v));
    countTotal();
}

void WOrder::changePrice()
{
    int row = ui->tblGoods->currentRow();
    if (row < 0) {
        return;
    }
    double currentPrice = ui->tblGoods->getDouble(row, 4);
    if (__c5config.shopDenyPriceChange() && !(currentPrice <0)) {
        return;
    }
    double price = DQty::getQty(tr("Price"), this);
    if (price < 0.001) {
        return;
    }
    ui->tblGoods->setDouble(row, 4, price);
    ui->tblGoods->setDouble(row, 5, ui->tblGoods->getDouble(row, 2) * price);
    countTotal();
}

int WOrder::rowCount()
{
    return ui->tblGoods->rowCount();
}

void WOrder::removeRow()
{
    int row = ui->tblGoods->currentRow();
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
    QString code = ui->tblGoods->getString(row, 0);
    QString name = ui->tblGoods->getString(row, 1);
    double qty = ui->tblGoods->getDouble(row, 2);
    ui->tblGoods->removeRow(row);
    ui->leCard->setText("0");
    ui->leCash->setText("0");
    ui->leChange->setText("0");
    C5Database db(__c5config.dbParams());
    db[":f_window"] = fWorking->fTab->currentIndex();
    db[":f_state"] = 0;
    db[":f_row"] = row;
    db[":f_station"] = hostinfo + ": " + hostusername();
    db.exec("update a_sale_temp set f_state=1 where f_station=:f_station and f_state=:f_state and f_window=:f_window and f_row=:f_row");
    db[":f_window"] = fWorking->fTab->currentIndex();
    db[":f_state"] = 0;
    db[":f_row"] = row;
    db[":f_station"] = hostinfo + ": " + hostusername();
    db.exec("update a_sale_temp set f_row=f_row-1 where f_station=:f_station and f_row>:f_row and f_state=:f_state and f_window=:f_window");
    db[":f_store"] = __c5config.defaultStore();
    db[":f_goods"] = code.toInt();
    db[":f_qty"] = qty;
    db.exec("update a_store_sale set f_qtyprogram=f_qtyprogram-:f_qty where f_store=:f_store and f_goods=:f_goods");
    C5LogSystem::writeEvent(QString("%1 #%2 %3:%4 %5").arg(tr("Remove row")).arg(row).arg(code).arg(name).arg(float_str(qty, 2)));
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

int WOrder::lastRow()
{
    return ui->tblGoods->rowCount() - 1;
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

void WOrder::setPartner(const QString &taxcode, int id, const QString &taxname)
{
    QString fullname = taxcode;
    if (!taxname.isEmpty()) {
        fullname += ", " + taxname;
    }
    ui->lePartner->setText(fullname);
    fPartner = id;
    ui->lbPartner->setVisible(true);
    ui->lePartner->setVisible(true);
    ui->lbDept->setVisible(true);
    ui->chDebt->setVisible(true);
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
            disc = fCardValue;
            break;
        default:
            total += ui->tblGoods->getDouble(i, 5);
            break;
        }
    }
    switch (fCardMode) {
    case CARD_TYPE_DISCOUNT: {
        ui->leDisc->setText(float_str(discount, 2));
        ui->lbDisc->setText(QString("%1 %2%").arg(tr("Discount")).arg(float_str(fCardValue * 100, 2)));
        break;
    }
    case CARD_TYPE_ACCUMULATIVE: {
        ui->leDisc->setText(float_str(discount, 2));
        ui->lbDisc->setText(QString("%1 %%2").arg(tr("Discount")).arg(float_str(fCardValue * 100, 2)));
        break;
    }
    case CARD_TYPE_COUNT_ORDER: {
        ui->lbDisc->setText(QString("%1").arg(tr("Visit counter")));
        break;
    }
    case CARD_TYPE_MANUAL:
        ui->lbDisc->setText(QString("%1").arg(tr("Discount")).arg(float_str(fCardValue, 2)));
        ui->leDisc->setText(float_str(fCardValue, 2));
        total -= fCardValue;
        break;
    default:
        break;
    }
    ui->leTotal->setDouble(total);
    C5LogSystem::writeEvent(QString("%1: %3").arg(tr("Total amount")).arg(float_str(total, 2)));
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

void WOrder::setQtyOfRow(int row, double qty)
{
    DbGoods g(ui->tblGoods->getInteger(row, 0));
    if (g.acceptIntegerQty()) {
        qty = trunc(qty);
    }
    double diffqty = qty - ui->tblGoods->getInteger(row, tr("Qty"));

    if (!g.isService() && __c5config.getValue(param_shop_dont_check_qty).toInt() == 0) {
        QString err;
        if (!checkQty(g.id(), diffqty, err)) {
            C5Message::error(err);
            return;
        }
    }

    ui->tblGoods->setDouble(row, 2, qty);
    ui->tblGoods->setDouble(row, 5, ui->tblGoods->getDouble(row, 4) * qty);
    C5LogSystem::writeEvent(QString("%1 %2:%3 %4").arg(tr("Change qty")).arg(g.scancode()).arg(g.goodsName()).arg(float_str(qty, 2)));
    countTotal();
}

C5TableWidget *WOrder::table()
{
    return ui->tblGoods;
}

bool WOrder::checkQty(int goods, double qty, QString &err)
{
    DbGoods gd(goods);
    int storegoods = gd.storeGoods();
    C5Database db(__c5config.dbParams());
    db[":f_store"] = __c5config.defaultStore();
    db[":f_goods"] = storegoods;
    db[":f_qty"] = qty;
    if (!db.exec("select f_qty-:f_qty-f_qtyreserve-f_qtyprogram as f_qty from a_store_sale where f_store=:f_store and f_goods=:f_goods ")) {
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

    db[":f_qty"] = qty;
    db[":f_store"] = __c5config.defaultStore();
    db[":f_goods"] = storegoods;
    db.exec("update a_store_sale set f_qtyprogram=f_qtyprogram+:f_qty where f_store=:f_store and f_goods=:f_goods");

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

void WOrder::on_leCash_textChanged(const QString &arg1)
{
    ui->leCash->disconnect();
    QString a = QString(arg1).replace(",", "");
    ui->leChange->setDouble(str_float(a) - ui->leTotal->getDouble());
    ui->leCash->setDouble(str_float(a));
    if (arg1.toDouble() < 0.01) {
        ui->leChange->setText("0");
    }
    connect(ui->leCash, &C5LineEdit::textChanged, this, &WOrder::on_leCash_textChanged);
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
    ui->leCode->clear();
    ui->leCode->setFocus();

    if (code.at(0).toLower() == '?' ) {
        if (code.at(1).toLower() == 'c') {
            code.remove(0, 2);
            fixCostumer(code);
            return;
        }
    }
    if (code.at(0).toLower() == '/') {
        code.remove(0, 1);
        discountRow(code);
        return;
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


    if (code.mid(0, 2) == "23") {
        if (code.length() != 13) {
            addGoods(id);
            return;
        }
        QString code2 = QString("%1").arg(code.mid(2, 5).toInt());
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
    } else {
        addGoods(id);
    }
}

void WOrder::on_leCustomerTaxpayerId_returnPressed()
{
    if (ui->lePartner->text().isEmpty()) {
        return;
    }
    C5Database db(__c5config.dbParams());
    db[":f_taxcode"] = ui->lePartner->text();
    db.exec("select f_id, f_taxname from c_partners where f_taxcode=:f_taxcode");
    if (db.nextRow()) {
        setPartner(ui->lePartner->text(), db.getInt("f_id"), db.getString(1));
    } else {
        db[":f_taxcode"] = ui->lePartner->text();
        int pid = db.insert("c_partners");
        setPartner(ui->lePartner->text(), pid, "");
    }
}

void WOrder::on_leCard_textChanged(const QString &arg1)
{
    ui->leCard->disconnect();
    QString a = QString(arg1).replace(",", "");
    if (str_float(a) + str_float(ui->leAdvance->text()) > str_float(ui->leTotal->text())) {
        ui->leAdvance->setText(0);
        if (str_float(a) > str_float(ui->leTotal->text())) {
            ui->leCard->setText(ui->leTotal->text());
        }
    } else {
        ui->leCard->setDouble(str_float(a));
    }
    connect(ui->leCard, &C5LineEdit::textChanged, this, &WOrder::on_leCard_textChanged);
}

void WOrder::on_leAdvance_textChanged(const QString &arg1)
{
    ui->leAdvance->disconnect();
    QString a = QString(arg1).replace(",", "");
    if (str_float(a) + str_float(ui->leCard->text()) > str_float(ui->leTotal->text())) {
        ui->leCard->setText(0);
        if (str_float(a) > str_float(ui->leTotal->text())) {
            ui->leAdvance->setText(ui->leTotal->text());
        }
    } else{
        ui->leAdvance->setDouble(str_float(a));
    }
    connect(ui->leAdvance, &C5LineEdit::textChanged, this, &WOrder::on_leAdvance_textChanged);
}
