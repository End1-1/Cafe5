#include "worder.h"
#include "ui_worder.h"
#include "c5database.h"
#include "c5config.h"
#include "printtaxn.h"
#include "dqty.h"
#include "c5message.h"
#include "c5permissions.h"
#include "c5printing.h"
#include "printreceipt.h"
#include "c5shoporder.h"
#include "working.h"
#include "c5utils.h"
#include "c5storedraftwriter.h"
#include <QInputDialog>

WOrder::WOrder(int saleType, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WOrder)
{
    ui->setupUi(this);
    fSaleType = saleType;
    fPartner = 0;
    fWorking = static_cast<Working*>(parent);
    fDateOpen = QDate::currentDate();
    fTimeOpen = QTime::currentTime();
    ui->tblGoods->setColumnWidths(ui->tblGoods->columnCount(), 0, 300, 80, 80, 80, 80, 80, 80);
    fCostumerId = 0;
    fCardValue = 0;
    ui->lbDisc->setVisible(false);
    ui->leDisc->setVisible(false);
    ui->lbCustomer->setVisible(false);
    ui->leCustomer->setVisible(false);
    ui->leCard->setValidator(new QDoubleValidator(0, 1000000000,2 ));
    ui->leCash->setValidator(new QDoubleValidator(0, 1000000000,2 ));
    ui->leChange->setValidator(new QDoubleValidator(0, 1000000000,2 ));
    ui->lbPartner->setVisible(false);
    ui->lePartner->setVisible(false);
}

WOrder::~WOrder()
{
    delete ui;
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
            C5Message::error(tr("Insufficient quantity") + "<br>" + float_str(totalQty - gg.fQty, 3));
            return;
        }
    }

    double price = fSaleType == SALE_RETAIL ? g.fRetailPrice : g.fWhosalePrice;
    int row = ui->tblGoods->addEmptyRow();
    ui->tblGoods->setString(row, 0, g.fCode);
    ui->tblGoods->setString(row, 1, g.fName + " " + g.fScanCode);
    ui->tblGoods->setDouble(row, 2, 1);
    ui->tblGoods->setString(row, 3, g.fUnit);
    ui->tblGoods->setDouble(row, 4, price);
    ui->tblGoods->setDouble(row, 5, price);
    ui->tblGoods->setInteger(row, 6, g.fTaxDept);
    ui->tblGoods->setString(row, 7, g.fAdgCode);
    ui->tblGoods->setCurrentItem(ui->tblGoods->item(row, 0));
    countTotal();
}

bool WOrder::writeOrder(bool tax)
{
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
        goods.append(g);
    }
    C5ShopOrder so;
    so.setPartner(fPartner, ui->lePartner->text());
    so.setDiscount(fCostumerId, fCardId, fCardMode, fCardValue);
    so.setParams(fDateOpen, fTimeOpen, fSaleType);
    return so.write(ui->leTotal->getDouble(), ui->leCard->getDouble(), ui->leDisc->getDouble(), tax, goods);
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
    fCostumerId = db.getInt("f_client");
    fCardId = db.getInt("f_id");
    fCardMode = db.getInt("f_mode");
    fCardValue = db.getDouble("f_value");
    db[":f_id"] = fCostumerId;
    db.exec("select * from c_partners where f_id=:f_id");
    if (!db.nextRow()) {
        return;
    }
    ui->leDisc->setVisible(true);
    ui->leCustomer->setVisible(true);
    ui->lbCustomer->setVisible(true);
    ui->lbDisc->setVisible(true);
    ui->leCustomer->setText(db.getString("f_firstname") + " " + db.getString("f_lastname"));
    countTotal();
}

void WOrder::changeQty()
{
    int row = ui->tblGoods->currentRow();
    if (row < 0) {
        return;
    }
    double qty = DQty::getQty(tr("Quantity"), this);
    if (qty < 0.001) {
        return;
    }
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
        if (g.fQty < totalQty) {
            C5Message::error(tr("Insufficient quantity") + "<br>" + float_str(totalQty - g.fQty, 3));
            return;
        }
    }
    ui->tblGoods->setDouble(row, 2, qty);
    ui->tblGoods->setDouble(row, 5, ui->tblGoods->getDouble(row, 4) * qty);
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
    ui->tblGoods->removeRow(row);
    ui->leCard->clear();
    ui->leCash->clear();
    ui->leChange->clear();
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
}

bool WOrder::returnFalse(const QString &msg, C5Database &db)
{
    db.rollback();
    db.close();
    C5Message::error(msg);
    return false;
}

void WOrder::on_leCash_textChanged(const QString &arg1)
{
    ui->leChange->setDouble(arg1.toDouble() - ui->leTotal->getDouble());
}

void WOrder::on_leCard_textChanged(const QString &arg1)
{
    if (arg1.toDouble() > ui->leTotal->getDouble()) {
        ui->leCard->setDouble(ui->leTotal->getDouble());
    }
}
