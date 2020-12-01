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
#include "c5storedoc.h"
#include "preorders.h"
#include "c5logsystem.h"
#include "printreceipt.h"
#include "selectstaff.h"
#include "c5shoporder.h"
#include "working.h"
#include "c5utils.h"
#include "taxprint.h"
#include "c5storedraftwriter.h"
#include "c5replication.h"
#include <QInputDialog>

#define col_qty 2
#define col_price 4
#define col_total 5
#define col_discount_value 11
#define col_discount_mode 12

WOrder::WOrder(int saleType, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WOrder)
{
    ui->setupUi(this);
    ui->wPreorder->setVisible(false);
    fSaleType = saleType;
    fPartner = 0;
    fWorking = static_cast<Working*>(parent);
    fDateOpen = QDate::currentDate();
    fTimeOpen = QTime::currentTime();
    ui->tblGoods->setColumnWidths(ui->tblGoods->columnCount(), 0, 300, 80, 80, 80, 80, 80, 80, 0, 0, 0, 80, 30);
    fCostumerId = 0;
    fCardValue = 0;
    fCardMode = 0;
    ui->lbDisc->setVisible(false);
    ui->leDisc->setVisible(false);
    ui->lbCustomer->setVisible(false);
    ui->leCustomer->setVisible(false);
    ui->leCard->setValidator(new QDoubleValidator(0, 1000000000, 2));
    ui->leCash->setValidator(new QDoubleValidator(0, 1000000000, 2));
    ui->leChange->setValidator(new QDoubleValidator(0, 1000000000, 2));
    ui->lbPartner->setVisible(false);
    ui->lePartner->setVisible(false);
    ui->lbDept->setVisible(false);
    ui->chDebt->setVisible(false);
    noImage();
    if (fSaleType == SALE_PREORDER) {
        setStyleSheet("background: #d0ffc1;");
        ui->wPreorder->setVisible(true);
    }
    ui->leAdvance->setValidator(new QDoubleValidator(0, 99999999, 2));
    connect(ui->leCard, &C5LineEdit::focusOut, [=](){
        C5LogSystem::writeEvent(QString("%1: %2").arg(tr("Card value")).arg(ui->leCard->text()));
    });
    connect(ui->leCash, &C5LineEdit::focusOut, [=](){
        C5LogSystem::writeEvent(QString("%1: %2").arg(tr("Cash value")).arg(ui->leCard->text()));
    });
}

WOrder::~WOrder()
{
    delete ui;
}

void WOrder::changeIshmarColor(const QString &c)
{
    ui->wIshmar->setStyleSheet(c);
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
    double totalQty = 1;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if (ui->tblGoods->getString(i, 0) == g.fCode) {
            totalQty += ui->tblGoods->getDouble(i, 2);
        }
    }
    Goods gg = fWorking->fGoods[g.fScanCode];
    if (__c5config.controlShopQty()) {
        if (gg.fQty < totalQty) {
            if (!gg.fIsService && gg.fUncomplectFrom == 0) {
                C5Message::error(tr("Insufficient quantity") + "<br>" + float_str(totalQty - gg.fQty, 3));
                return;
            }
            if (gg.fUncomplectFrom > 0) {
                if (fWorking->fUncomplectGoods.contains(gg.fCode.toInt())) {
                    UncomplectGoods ug = fWorking->fUncomplectGoods[gg.fCode.toInt()];
                    Goods gu = fWorking->fGoods[fWorking->fGoodsCodeForPrint[ug.uncomplectGoods]];
                    if (C5Message::question(tr("Insufficient quantity, but can uncomplect from ") + "<br>" + gu.fName) == QDialog::Accepted) {
                        C5StoreDoc d(__c5config.replicaDbParams());
                        d.setMode(C5StoreDoc::sdOutput);
                        d.setStore(0, __c5config.defaultStore());
                        d.setComment(tr("Uncomplect for sale"));
                        d.addByScancode(gu.fScanCode, "1", "");
                        d.saveDoc();
                        double price = d.total() / ug.qty;
                        C5StoreDoc *di = new C5StoreDoc(__c5config.replicaDbParams());
                        di->setMode(C5StoreDoc::sdInput);
                        di->setStore(__c5config.defaultStore(), 0);
                        di->setComment(tr("Uncomplect for sale"));
                        di->addByScancode(gg.fScanCode, QLocale().toString(ug.qty), QLocale().toString(price));
                        di->saveDoc();
                        di->deleteLater();
                        fWorking->makeWGoods();
                        addGoods(g);
                        return;
                    } else {
                        C5Message::error(tr("Insufficient quantity") + "<br>" + float_str(totalQty - gg.fQty, 3));
                        return;
                    }
                } else {
                    C5Message::error(tr("Insufficient quantity") + "<br>" + float_str(totalQty - gg.fQty, 3));
                    return;
                }
            } else {
                if (gg.fIsService) {
                    addGoodsToTable(g);
                    return;
                } else {
                    C5Message::error(tr("Insufficient quantity") + "<br>" + float_str(totalQty - gg.fQty, 3));
                    return;
                }
            }
        }
    }
    addGoodsToTable(g);
}

void WOrder::addGoodsToTable(const Goods &g)
{
    double price = 0;
    switch (fSaleType) {
    case SALE_RETAIL:
    case SALE_PREORDER:
        price = g.fRetailPrice;
        break;
    case SALE_WHOSALE:
        price = g.fWhosalePrice;
        break;
    }
    ls(QString("%1: %2, %3: %4, %5: %6")
       .arg(tr("New goods with code"))
       .arg(g.fScanCode).arg(tr("name"))
       .arg(g.fName).arg(tr("retail price"))
       .arg(g.fRetailPrice));
    int row = ui->tblGoods->addEmptyRow();
    ui->tblGoods->setString(row, 0, g.fCode);
    ui->tblGoods->setString(row, 1, g.fName + " " + g.fScanCode);
    ui->tblGoods->setDouble(row, 2, 1);
    ui->tblGoods->item(row, 2)->fDecimals = 3;
    ui->tblGoods->setString(row, 3, g.fUnit);
    ui->tblGoods->setDouble(row, 4, price);
    ui->tblGoods->setDouble(row, 5, price);
    ui->tblGoods->setInteger(row, 6, g.fTaxDept);
    ui->tblGoods->setString(row, 7, g.fAdgCode);
    ui->tblGoods->setString(row, 8, "");
    ui->tblGoods->setInteger(row, 9, g.fUnitCode);
    ui->tblGoods->setInteger(row, 10, g.fIsService ? 1 : 0);
    ui->tblGoods->setDouble(row, col_discount_value, 0);
    ui->tblGoods->setString(row, col_discount_mode, "");
    ui->tblGoods->setCurrentItem(ui->tblGoods->item(row, 0));
    countTotal();

    ImageLoader *il = new ImageLoader(g.fScanCode, this);
    connect(il, SIGNAL(imageLoaded(QPixmap)), this, SLOT(imageLoaded(QPixmap)));
    connect(il, SIGNAL(noImage()), this, SLOT(noImage()));
    il->start();
}

bool WOrder::writeOrder(bool tax)
{
    QElapsedTimer t;
    t.start();

    if (__c5config.shopDifferentStaff() && fWorking->fCurrentUsers.count() > 0) {
        SelectStaff ss(fWorking);
        ss.exec();
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
        goods.append(g);
    }
    C5ShopOrder so;
    so.setPartner(fPartner, ui->lePartner->text());
    so.setDiscount(fCostumerId, fCardId, fCardMode, fCardValue);
    so.setParams(fDateOpen, fTimeOpen, fSaleType);
    C5LogSystem::writeEvent(QString("%1. %2:%3, %4:%5, %6:%7, %8:%9").arg(tr("Before write")).arg(tr("Total")).arg(ui->leTotal->text()).arg(tr("Card")).arg(ui->leCard->text()).arg(tr("Advance")).arg(ui->leAdvance->text()).arg(tr("Dicount")).arg(ui->leDisc->text()));
    bool w = so.write(ui->leTotal->getDouble(), ui->leCard->getDouble(), ui->leAdvance->getDouble(), ui->leDisc->getDouble(), tax, goods, fCardValue, fCardMode);
    C5Database db(__c5config.dbParams());
    if (w) {
        foreach (const IGoods &g, goods) {
            if (g.isService) {
                continue;
            }
            db[":f_goods"] = g.goodsId;
            db[":f_store"] = __c5config.defaultStore();
            db[":f_qty"] = g.goodsQty;
            db.exec("update a_store_temp set f_qty=f_qty-:f_qty where f_goods=:f_goods and f_store=:f_store");
            fWorking->decQty(g);
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

        auto *r = new C5Replication();
        r->start(SLOT(uploadToServer()));
    }
    C5LogSystem::writeEvent(QString("%1. %2:%3ms, %4:%5, %6").arg(tr("Order saved")).arg(tr("Elapsed")).arg(t.elapsed()).arg(tr("Order number")).arg(so.fHallId).arg(so.fHeader));
    return w;
}

bool WOrder::writePreorder()
{
    if (ui->tblGoods->rowCount() == 0) {
        C5Message::error(tr("Empty order"));
        return false;
    }
    if (ui->leContactName->text().isEmpty()) {
        C5Message::error(tr("Customer name cannot be empty"));
        return false;
    }
    bool r = true;
    bool u = true;
    C5Database db(__c5config.dbParams());
    db.startTransaction();
    db[":f_contact"] = ui->leContactName->text();
    db.exec("select * from c_partners where f_contact=:f_contact and f_phone=:f_phone");
    if (db.nextRow()) {
        fPartner = db.getInt("f_id");
    } else {
        db[":f_contact"] = ui->leContactName->text();
        db[":f_phone"] = ui->leContactPhone->text();
        fPartner = db.insert("c_partners");
    }
    if (fPreorderUUID.isEmpty()) {
        u = false;
        fPreorderUUID = db.uuid_bin();
    }
    db[":f_id"] = fPreorderUUID;
    db[":f_state"] = 1;
    db[":f_datecreate"] = QDate::currentDate();
    db[":f_timecreate"] = QTime::currentTime();
    db[":f_staff"] = __userid;
    db[":f_datefor"] = ui->dePreorerDate->date();
    db[":f_timefor"] = ui->tmTime->time();
    db[":f_total"] = ui->leTotal->getDouble();
    db[":f_advance"] = ui->leAdvance->getDouble();
    db[":f_customer"] = fPartner;
    db[":f_info"] = ui->tePreorderInfo->toPlainText();
    if (u) {
        r = db.update("op_header", "f_id", fPreorderUUID);
    } else {
        r = db.insert("op_header", false);
    }
    if (!r) {
        db.rollback();
        C5Message::error(db.fLastError);
        return false;
    }
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        u = true;
        QByteArray bid = ui->tblGoods->getData(i, 8).toByteArray();
        if (bid.isEmpty()) {
            u = false;
            bid = db.uuid_bin();
        }
        db[":f_id"] = bid;
        db[":f_header"] = fPreorderUUID;
        db[":f_goods"] = ui->tblGoods->getInteger(i, 0);
        db[":f_qty"] = ui->tblGoods->getDouble(i, 2);
        db[":f_price"] = ui->tblGoods->getDouble(i, 4);
        if (u) {
            r = db.update("op_body", "f_id", bid);
        } else {
            r = db.insert("op_body", false);
        }
        if (!r) {
            db.rollback();
            C5Message::error(db.fLastError);
            return false;
        }
    }
    db.commit();
    C5Message::info(tr("Saved"));
    return r;
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
    double qty = DQty::getQty(tr("Price"), this);
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
    QString code = ui->tblGoods->getString(row, 0);
    QString name = ui->tblGoods->getString(row, 1);
    double qty = ui->tblGoods->getDouble(row, 2);
    ui->tblGoods->removeRow(row);
    ui->leCard->clear();
    ui->leCash->clear();
    ui->leChange->clear();
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
    double totalQty = qty;
    QString goodsCode = fWorking->fGoodsCodeForPrint[ui->tblGoods->getInteger(row, 0)];
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if (i == row) {
            continue;
        }
        if (ui->tblGoods->getString(i, 0) == goodsCode) {
            totalQty += ui->tblGoods->getDouble(i, 2);
        }
    }

    Goods g = fWorking->fGoods[goodsCode];
    if (__c5config.controlShopQty()) {
        if (!g.fIsService && g.fQty < totalQty) {
            C5Message::error(tr("Insufficient quantity") + "<br>" + float_str(totalQty - g.fQty, 3));
            return;
        }
    }
    ui->tblGoods->setDouble(row, 2, qty);
    ui->tblGoods->setDouble(row, 5, ui->tblGoods->getDouble(row, 4) * qty);
    C5LogSystem::writeEvent(QString("%1 %2:%3 %4").arg(tr("Change qty")).arg(g.fCode).arg(g.fName).arg(float_str(qty, 2)));
    countTotal();
}

void WOrder::on_leCash_textChanged(const QString &arg1)
{
    ui->leChange->setDouble(arg1.toDouble() - ui->leTotal->getDouble());
}

void WOrder::on_leCard_textChanged(const QString &arg1)
{
    if (arg1.toDouble() + ui->leAdvance->getDouble() > ui->leTotal->getDouble()) {
        ui->leCard->setDouble(ui->leTotal->getDouble() - ui->leAdvance->getDouble());
    }
}

void WOrder::imageLoaded(const QPixmap &img)
{
    ui->lbGoodsImage->setPixmap(img.scaled(ui->lbGoodsImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->wimage->setVisible(true);
}

void WOrder::noImage()
{
    ui->wimage->setVisible(false);
}

void WOrder::on_btnInfo_clicked()
{
    QString info = QString("Ctrl+S: %1<br>Ctrl+I: %2<br>Ctrl+O: %3<br>Ctrl+T: %4<br>Ctrl+A: %5<br>Ctrl+H: %6")
            .arg(tr("Search goods in the storages"))
            .arg(tr("Input staff at the work"))
            .arg(tr("Output staff from the work"))
            .arg(tr("Total today"))
            .arg(tr("Preorder"))
            .arg(tr("Show log"));
    C5Message::info(info);
}

void WOrder::on_btnOpenOrder_clicked()
{
    Preorders *p = new Preorders(this);
    if (p->exec() == QDialog::Accepted) {
        ui->tblGoods->clearContents();
        ui->tblGoods->setRowCount(0);
        countTotal();
        C5Database db(__c5config.replicaDbParams());
        db[":f_id"] = p->fUUID;
        db.exec("select o.f_id, o.f_datecreate, o.f_timecreate, o.f_datefor, o.f_timefor, "
                "c.f_contact, c.f_phone, o.f_total, o.f_advance, o.f_info, o.f_customer, o.f_advancetax "
                "from op_header o "
                "left join c_partners c on c.f_id=o.f_customer "
                "where o.f_id=:f_id ");
        if (!db.nextRow()) {
            C5Message::error(tr("Invalid preorder id"));
            return;
        }
        double adv = db.getDouble("f_advance");
        ui->dePreorerDate->setDate(db.getDate("f_datefor"));
        ui->tmTime->setTime(db.getTime("f_timefor"));
        ui->leAdvance->setDouble(adv);
        ui->tePreorderInfo->setText(db.getString("f_info"));
        ui->leContactName->setProperty("id", db.getInt("f_customer"));
        ui->leContactName->setText(db.getString("f_contact"));
        ui->leContactPhone->setText(db.getString("f_phone"));
        ui->btnPrintTaxPrepaid->setEnabled(db.getInt("f_advancetax") == 0);
        ui->leAdvance->setEnabled(db.getInt("f_advancetax") == 0);
        db[":f_header"] = p->fUUID;
        db.exec("select og.f_id, og.f_goods, concat(g.f_name, ' ' , g.f_scancode) as f_name, og.f_qty, "
                "u.f_name as f_unitname, og.f_price, og.f_price*og.f_qty as f_total, gt.f_taxdept, "
                "gt.f_adgcode, '', g.f_unit "
                "from op_body og "
                "inner join c_goods g on g.f_id=og.f_goods "
                "inner join c_units u on u.f_id=g.f_unit "
                "inner join c_groups gt on gt.f_id=g.f_group "
                "where og.f_header=:f_header");
        while (db.nextRow()) {
            int row = ui->tblGoods->addEmptyRow();
            ui->tblGoods->setInteger(row, 0, db.getInt("f_goods"));
            ui->tblGoods->setString(row, 1, db.getString("f_name"));
            ui->tblGoods->setDouble(row, 2, db.getDouble("f_qty"));
            ui->tblGoods->setString(row, 3, db.getString("f_unitname"));
            ui->tblGoods->setDouble(row, 4, db.getDouble("f_price"));
            ui->tblGoods->setDouble(row, 5, db.getDouble("f_total"));
            ui->tblGoods->setString(row, 6, db.getString("f_taxdept"));
            ui->tblGoods->setString(row, 7, db.getString("f_adgcode"));
            ui->tblGoods->setString(row, 8, db.getString("f_id"));
            ui->tblGoods->setInteger(row, 9, db.getInt("f_unit"));
            ui->tblGoods->setCurrentItem(ui->tblGoods->item(row, 0));
        }
        fPreorderUUID = p->fUUID;
        countTotal();
        ui->leAdvance->setDouble(adv);
    }
    p->deleteLater();
}

void WOrder::on_btnSave_clicked()
{
    if (fSaleType == SALE_PREORDER) {
        writePreorder();
    }
}

void WOrder::on_btnPrintTaxPrepaid_clicked()
{
    if (fPreorderTax > 0) {
        C5Message::error(tr("Tax already printed"));
        return;
    }
    if (fPreorderUUID.isEmpty()) {
        C5Message::error(tr("Save first"));
        return;
    }
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(), C5Config::taxCashier(), C5Config::taxPin(), this);
    QString inJson;
    QString outJson;
    QString err;
    int result = pt.printAdvanceJson(ui->rbCash->isChecked() ? ui->leAdvance->getDouble() : 0, ui->rbCard->isChecked() ? ui->leAdvance->getDouble() : 0, inJson, outJson, err);
    C5Database db(__c5config.replicaDbParams());
    db[":f_id"] = db.uuid();
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db[":f_mark"] = "ADVANCE";
    db[":f_message"] = outJson;
    db.insert("o_tax_debug");
    if (result == pt_err_ok) {
        QString ofirm, ohvhh, ofiscal, onumber, osn, oaddress, odevnum, otime;
        PrintTaxN::parseResponse(outJson, ofirm, ohvhh, ofiscal, onumber, osn, oaddress, odevnum, otime);
        fPreorderTax = onumber.toInt();
        db[":f_advancetax"] = onumber;
        db.update("op_header", "f_id", fPreorderUUID);
        C5Message::info(tr("Printed"));
        ui->btnPrintTaxPrepaid->setEnabled(false);
        ui->leAdvance->setEnabled(false);
    } else {
        C5Message::error(outJson);
    }
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

void WOrder::on_leAdvance_textChanged(const QString &arg1)
{
    if (arg1.toDouble() > ui->leTotal->getDouble()) {
        ui->leAdvance->setDouble(ui->leTotal->getDouble());
    }
}

void WOrder::on_btnPrintManualTax_clicked()
{
    TaxPrint *tp = new TaxPrint();
    tp->exec();
    tp->deleteLater();
}
