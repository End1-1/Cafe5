#include "worder.h"
#include <QDir>
#include <QFile>
#include <QInputDialog>
#include <QPainter>
#include <QPointer>
#include <QPrinterInfo>
#include <QScrollBar>
#include <QSettings>
#include <QThread>
#include "c5checkbox.h"
#include "c5message.h"
#include "c5permissions.h"
#include "c5printing.h"
#include "c5replacecharacter.h"
#include "c5structtableview.h"
#include "c5user.h"
#include "c5utils.h"
#include "dict_payment_type.h"
#include "dlgpaymentchoose.h"
#include "dlgshopcustomer.h"
#include "dqty.h"
#include "goodscols.h"
#include "httplite.h"
#include "logwriter.h"
#include "ninterface.h"
#include "nloadingdlg.h"
#include "printreceiptgroup.h"
#include "printtaxn.h"
#include "selectprinters.h"
#include "selectstaff.h"
#include "struct_goods_item.h"
#include "struct_workstationitem.h"
#include "ui_worder.h"
#include "wcustomerdisplay.h"
#include "working.h"

WOrder::WOrder(C5User *user, int saleType, WCustomerDisplay *customerDisplay, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WOrder)
{
    ui->setupUi(this);
    ui->leTotal->setText("0");

    fCustomerDisplay = customerDisplay;
    fUser = user;
    fGiftCard = 0;
    fWorking = static_cast<Working *>(parent);
    setDiscinfoVisibility(false);
    noImage();
    QSettings s(_ORGANIZATION_, _APPLICATION_ + QString("\\") + _MODULE_);
    ui->tblData->setColumnWidth(col_action, 40);
    ui->tblData->setColumnWidth(col_barcode, 130);
    ui->tblData->setColumnWidth(col_group, 150);
    ui->tblData->setColumnWidth(col_name, 300);
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
    ui->tblData->setItemDelegateForColumn(col_action, new CustomDelegate());

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
    //todo db;
    QString err;
}

WOrder::~WOrder()
{
    delete ui;
}

void WOrder::updateCustomerDisplay(WCustomerDisplay *cd)
{
    fCustomerDisplay = cd;

    if (fCustomerDisplay) {
        fCustomerDisplay->clear();

        for (int i = 0; i < mOrder.dishes.size(); i++) {
            auto const &g = mOrder.dishes.at(i);
            fCustomerDisplay->addRow(g.dishName,
                                     float_str(g.qty, 2),
                                     float_str(g.price, 2),
                                     float_str(g.total(false), 2),
                                     float_str(g.discountFactor(), 2));
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
    changeQty();
}

void WOrder::keyAsterix()
{
    if (ui->leCode->text().length() > 1) {
        return;
    }

    clearCode();
    changePrice();
}

void WOrder::writeOrder(std::function<void()> nextStep)
{
    if (ui->btnF4->isChecked()) {
        if (ui->leTIN->text().length() != 8) {
            C5Message::error(tr("Invalid taxpayer tin"));
            return;
        }
    }

    if (mOrder.isEmpty()) {
        C5Message::error(tr("Empty order"));
        return;
    }

    // if (__c5config.cashId() == 0) {
    //     C5Message::error(tr("Cashdesk for cash not defined"));
    //     return;
    // }

    // if (__c5config.nocashId() == 0) {
    //     C5Message::error(tr("Cashdesk for card not defined"));
    //     return;
    // }

    //TODO
    // if (fOHeader.saleType == -1 && fOHeader.partner == 0) {
    //     C5Message::error(tr("Partner not defined"));
    //     return;
    // }

    bool prepaidReadonly = false;

    if (ui->leUseAccumulated->getDouble() > 0) {
        //fOHeader.amountPrepaid = ui->leUseAccumulated->getDouble();
        prepaidReadonly = true;
    }

    for (int i = 0; i < mOrder.dishes.size(); i++) {
        auto const &g = mOrder.dishes.at(i);
        if (g.state != 1) {
            continue;
        }

        if (g.qty < 0.0001) {
            C5Message::error(tr("Invalid qty"));
            return;
        }

        if (g.price < 0) {
            C5Message::error(tr("Invalid price"));
            return;
        }
    }

    bool _printFiscal = true; //TODO__c5config.alwaysOfferTaxPrint() || ui->btnF4->isChecked();

    //TODO replace prepaid max amount
    double cash = 0, card = 0, idram = 0, bank = 0, telcell = 0, debt = 0, prepaid = 0, cashin = 0, change = 0;
    if (ui->btnF5->isChecked()) {
        debt = mOrder.totalDue;
    }
    if (!DlgPaymentChoose::getValues(
            fUser, mOrder.totalDue, cash, card, idram, telcell, bank, prepaid, debt, cashin, change, _printFiscal, prepaidReadonly, 0)) {
        return;
    }

    QElapsedTimer t;
    t.start();

    //TODO
    int partner = 0;
    if ((debt > 0.001 || bank > 0.001) && partner == 0) {
        C5Message::error(tr("Debt impossible on unknown partner"));
        return;
    }

    SelectStaff ss(fWorking, fUser);
    int worker = 0;

    //TODO
    // if (__c5config.shopDifferentStaff() && fWorking->fCurrentUsers.count() > 0) {
    //     if (ss.exec() == QDialog::Rejected) {
    //         return;
    //     }

    //     worker = ss.mUserId;
    // }

    FiscalMachine fm = getFiscalMachine(mWorkStation.fiscalMachineId());

    if (fm.id == 0) {
        _printFiscal = false;
    }

    QPointer<WOrder> self(this);
    NInterface::query1("/engine/v2/waiter/order/set-amounts",
                       self->fUser->mSessionKey,
                       self,
                       {{"id", self->mOrder.id},
                        {"f_amount_cash", cash},
                        {"f_amount_card", card},
                        {"f_amount_bank", bank},
                        {"f_amount_idram", idram},
                        {"f_amount_debt", debt},
                        {"f_amount_telcell", telcell},
                        {"f_amount_prepaid", prepaid}},
                       [=](const QJsonObject &jdoc) {
                           if (!self) {
                               return;
                           }
                           auto closeOrderFunc = [=](const QJsonObject &fiscalInfo) {
                               if (!self) {
                                   return;
                               }

                               NInterface::query1("/engine/v2/waiter/order/close-order",
                                                  self->fUser->mSessionKey,
                                                  self,
                                                  {{"id", self->mOrder.id},
                                                   {"fiscal", fiscalInfo},
                                                   {"cashbox_id", mWorkStation.cashboxId()}},
                                                  [self](const QJsonObject &jdoc) {
                                                      self->parseOrder(jdoc);
                                                      self->printPrecheck();
                                                      emit self->orderSaved(self);
                                                  });
                           };

                           self->parseOrder(jdoc);
                           if (_printFiscal) {
                               printFiscal([=](const QJsonObject &jtax) { closeOrderFunc(jtax); });
                           } else {
                               closeOrderFunc({});
                           }
                       });
}

void WOrder::fixCostumer(const QString &code)
{
    //TODO
    //  todo db;
    //  db[":f_code"] = code;
    //  db.exec("select * from b_cards_discount where f_code=:f_code");

    //  if (!db.nextRow()) {
    //      return;
    //  }

    //  if (QDate::currentDate() > db.getDate("f_dateend")) {
    //      C5Message::error(tr("Cards was expired"));
    //      return;
    //  }

    // // fOHeader.partner = db.getInt("f_client");
    //  fBHistory.card = db.getInt("f_id");
    //  fBHistory.type = db.getInt("f_mode");
    //  fBHistory.value = db.getDouble("f_value");
    //  db[":f_id"] = fOHeader.partner;
    //  db.exec("select * from c_partners where f_id=:f_id");

    //  if (!db.nextRow()) {
    //      return;
    //  }

    //  if (__c5config.getValue(param_auto_discount) != code) {
    //      //TODO
    //      // if(!checkDiscountRight()) {
    //      //     return;
    //      // }
    //  }

    //  if (fBHistory.value < 0) {
    //      double v;

    //      if (!getDiscountValue(fBHistory.type, v)) {
    //          fBHistory.card = 0;
    //          fBHistory.value = 0;
    //          fBHistory.data = 0;
    //          return;
    //      }

    //      fBHistory.value = v;

    //      if (fBHistory.type == CARD_TYPE_DISCOUNT) {
    //          fBHistory.value = v / 100;
    //      } else {
    //          fBHistory.value = v;
    //      }
    //  }

    //  for (int i = 0; i < fOGoods.count(); i++) {
    //      OGoods &g = fOGoods[i];
    //      g.discountMode = fBHistory.type;
    //      g.discountFactor = fBHistory.value;
    //  }

    // countTotal();
}

void WOrder::changeQty()
{
    int row = ui->tblData->currentRow();

    if (row < 0) {
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

    setQtyOfRow(row, qty);
}

void WOrder::changePrice()
{
    int row = ui->tblData->currentRow();

    if (row < 0) {
        return;
    }

    auto &g = mOrder.dishes[row];
    double currentPrice = g.price;

    //TODO
    // if (__c5config.shopDenyPriceChange() && !(currentPrice < 0)) {
    //     return;
    // }

    double price = DQty::getQty(tr("Price"), 0, this);

    if (price < 0.001) {
        return;
    }

    g.price = price;
}

void WOrder::changePrice(double price)
{
    int row = ui->tblData->currentRow();

    if (row < 0) {
        return;
    }

    mOrder.dishes[row].price = price;
}

int WOrder::rowCount()
{
    return mOrder.dishes.size();
}

void WOrder::removeRow()
{
    int row = ui->tblData->currentRow();

    if (row < 0) {
        if (ui->tblData->rowCount() > 0) {
            row = 0;
        } else {
            return;
        }
    }

    C5User *tmp = fUser;

    if (!tmp->check(cp_t12_remove_order_row)) {
        QString password = QInputDialog::getText(this, tr("Password"), tr("Password"), QLineEdit::Password);
        C5User *tmp = new C5User;

        if (tmp->error().isEmpty()) {
        } else {
            C5Message::error(tmp->error());
            delete tmp;
            return;
        }
    }

    auto const &d = ui->tblData->getData(row, 0).value<WaiterDish>();
    NInterface::query1("/engine/v2/waiter/order/set-dish-qty",
                       fUser->mSessionKey,
                       this,
                       {{"id", d.id},
                        {"order_id", mOrder.id},
                        {"dish", d.dishId},
                        {"dish_name", d.dishName},
                        {"new_state", 3},
                        {"data", d.data},
                        {"new_qty", d.qty},
                        {"restore_stoplist", d.qty},
                        {"remove_reason", ""}},
                       [=](const QJsonObject &jdoc) { parseOrder(jdoc); });
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
    //TODO
    // if (__c5config.getValue(param_shop_deny_qtychange).toInt() == 0) {
    //     int row = ui->tblData->currentRow();

    //     if (row < 0) {
    //         return;
    //     }

    //     double qty = DQty::getQty(tr("Box"), 0, this);

    //     if (qty < 0.001) {
    //         return;
    //     }

    //     setQtyOfRow(row, qty);
    // }
}

void WOrder::printPrecheck()
{
    int bs = 20;
    QFont font(qApp->font());
    font.setPointSize(bs);
    QString printerName = mWorkStation.defaultPrinter();
    C5Printing p;
    QPrinterInfo pi = QPrinterInfo::printerInfo(printerName);
    QPrinter printer(pi);
    printer.setPageSize(QPageSize::Custom);
    printer.setFullPage(false);
    QRectF pr = printer.pageRect(QPrinter::DevicePixel);
    constexpr qreal SAFE_RIGHT_MM = 4.0;
    qreal safePx = SAFE_RIGHT_MM * printer.logicalDpiX() / 25.4;
    p.setSceneParams(pr.width() - safePx, pr.height(), printer.logicalDpiX());
    p.setFont(font);
    p.setFontSize(bs);
    QString logoFile = qApp->applicationDirPath() + "/logo_receipt.png";

    if (QFile::exists(logoFile)) {
        p.image(logoFile, Qt::AlignHCenter);
        p.br();
    }

    switch (mOrder.state) {
    case ORDER_STATE_OPEN:
    case ORDER_STATE_CLOSE:
        p.ltext(tr("Receipt"));
        break;

    case ORDER_STATE_PREORDER:
        p.ltext(tr("Preorder"));
        break;

    default:
        p.ltext(QString::number(mOrder.state) + ": " + tr("Error in state"));
        break;
    }

    p.rtext(mOrder.receiptNumber);
    p.br();
    QJsonObject jtax = mOrder.fiscal();

    if (!jtax.isEmpty()) {
        // jtax = QJsonDocument::fromJson(jtax.value("out").toString().toUtf8()).object();
        p.ltext(jtax["taxpayer"].toString(), 0);
        p.br();
        p.ltext(jtax["address"].toString(), 0);
        p.br();
        p.ltext(tr("TIN"), 0);
        p.rtext(jtax["tin"].toString());
        p.br();
        p.ltext(tr("Device number"), 0);
        p.rtext(jtax["crn"].toString());
        p.br();
        p.ltext(tr("Serial"), 0);
        p.rtext(jtax["sn"].toString());
        p.br();
        p.ltext(tr("Fiscal"), 0);
        p.rtext(jtax["fiscal"].toString());
        p.br();
        p.ltext(tr("Receipt number"), 0);
        p.rtext(QString::number(jtax["rseq"].toInt()));
        p.br();
        p.ltext(tr("Date"), 0);
        p.rtext(QDateTime::fromMSecsSinceEpoch(jtax["time"].toDouble()).toString(FORMAT_DATETIME_TO_STR));
        p.br();
        p.ltext(tr("(F)"), 0);
        p.br();
    }

    p.br(1);
    p.ltext(tr("Table"), 0);
    p.rtext(QString("%1/%2").arg(mOrder.hallName, mOrder.tableName));
    p.br();
    p.line(2);
    p.br(2);
    //p.setFontSize(bs - 4);
    p.ltext(tr("Name"), 0);
    p.ltext(tr("Qty"), 33);
    p.ltext(tr("Price"), 41);
    p.rtext(tr("Amount"));
    p.br();
    p.br(2);
    p.line();
    p.br(1);
    bool noservice = false, nodiscount = false, complimentary = false;

    for (int i = 0; i < mOrder.precheckDishes.size(); i++) {
        p.setFontSize(bs - 2);
        auto dish = mOrder.precheckDishes.at(i);

        if (dish.state != DISH_STATE_OK) {
            continue;
        }

        if (!dish.adgtCode().isEmpty()) {
            p.ltext(QString("%1: %2").arg(tr("Class"), dish.adgtCode()), 0);
            p.br();
        }

        QString name = dish.translated();

        p.ltext(name, 0, 35);
        p.ltext(float_str(dish.qty, 2), 33, 8);
        p.ltext(float_str(dish.price, 2), 41, 12);
        p.rtext(float_str(dish.total(mOrder.state == ORDER_STATE_PREORDER), 2));
        p.br();
        p.br(2);
        p.line();
        p.br(1);
    }

    p.setFontSize(bs - 2);

    p.setFontSize(bs + 2);
    p.ltext(tr("Subtotal"), 0);
    p.rtext(float_str(mOrder.subTotal(), 2));
    p.br();

    if (mOrder.serviceFactor() > 0) {
        //p.ltext(QString("%1 %2%").arg(tr("Service")).arg(mOrder.serviceFactor() * 100), 0);
        //p.rtext(float_str(mOrder.serviceAmount(), 2));
        p.ltext(QString("%1").arg(tr("Service")));
        p.rtext("+" + float_str(mOrder.serviceFactor() * 100, 2) + "%");
        p.br();
    }

    if (mOrder.discountFactor() > 0) {
        // p.ltext(QString("%1 %2%").arg(tr("Discount"), float_str(mOrder.discountFactor() * 100, 2)), 0);
        // p.rtext(float_str(mOrder.discountAmount(), 2));
        p.ltext(QString("%1").arg(tr("Discount")));
        p.rtext("-" + float_str(mOrder.discountFactor() * 100, 2) + "%");
        p.br();
    }

    if (mOrder.prepaidAmount() > 0) {
        p.ltext(tr("Prepaid amount"), 0);
        p.setFontSize(bs);
        p.rtext(float_str(mOrder.prepaidAmount() * -1, 2));
        p.br();
    }

    p.ltext(tr("Total due"));
    p.rtext(float_str(mOrder.totalDue, 2));
    p.br();
    p.br();
    auto printPaymentFunc = [this, &p](int id) {
        if (mOrder.payment(payment_fields[id]) > 0) {
            p.ltext(QCoreApplication::translate("PaymentType", payment_names[id]), 0);
            p.rtext(float_str(mOrder.payment(payment_fields[id]), 2));
            p.br();
        }
    };

    for (auto pt : payment_types) {
        printPaymentFunc(pt);
    }

    if (mOrder.amountPaid() - mOrder.totalDue > 0) {
        p.br();
        p.ltext(tr("Amount paid"), 0);
        p.rtext(float_str(mOrder.amountPaid(), 2));
        p.br();
        p.ltext(tr("Change"), 0);
        p.rtext(float_str(mOrder.amountPaid() - mOrder.totalDue, 2));
        p.br();
    }

    p.br();
    p.setFontSize(bs - 2);
    p.ltext(tr("Thank you for visit!"), 0);
    p.br();

    if (mOrder.state == ORDER_STATE_OPEN || mOrder.state == ORDER_STATE_CLOSE) {
        p.ltext(QString("%1: %2").arg(tr("Sample")).arg(mOrder.printCount()));
    }

    p.br();
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();

    if (mWorkStation.printServer().isEmpty() == false) {
        HttpLite *http = new HttpLite(this);
        QJsonObject json;
        json["print_data"] = p.jsonData();
        json["printer_name"] = printerName;

        http->post(mWorkStation.printServer(), json);
    } else {
        p.print(printer);
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

void WOrder::setPartner(PartnerItem pi)
{
    ui->leTIN->setText(pi.tin);
    QStringList parts;
    if (!pi.taxName.isEmpty()) {
        parts.append(pi.taxName);
    }
    if (!pi.contactName.isEmpty()) {
        parts.append(pi.contactName);
    }
    if (!pi.phone.isEmpty()) {
        parts.append(pi.phone);
    }

    ui->leCustomer->setText(parts.join(", "));
    ui->btnF5->setVisible(true);

    if (mWorkStation.quickDebtPartnerId() == pi.id) {
        ui->btnF5->setChecked(true);
    }
}

bool WOrder::setQtyOfRow(int row, double qty)
{
    auto const &og = ui->tblData->getData(row, 0).value<WaiterDish>();
    if (og.id.isEmpty()) {
        C5Message::error("Error og.id.isEmpty == true");
        return false;
    }

    if (og.isPiece()) {
        qty = trunc(qty);
    }

    NInterface::query1("/engine/v2/waiter/order/set-dish-qty",
                       fUser->mSessionKey,
                       this,
                       {{"id", og.id},
                        {"remove_emarks", false},
                        {"dish", og.dishId},
                        {"dish_name", og.dishName},
                        {"new_qty", qty},
                        {"new_state", 1},
                        {"data", og.data},
                        {"order_id", mOrder.id}},
                       [=](const QJsonObject &jdoc) { parseOrder(jdoc); });

    return true;
}

bool WOrder::setPriceOfRow(int row, double price)
{
    mOrder.dishes[row].price = price;
    return true;
}

C5ClearTableWidget *WOrder::table()
{
    return ui->tblData;
}

void WOrder::noImage()
{
    //ui->wimage->setVisible(false);
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
    QString code = ui->leCode->text();
    if (!code.isEmpty()) {
        if (code.first(1) == ";") {
            code.removeFirst();
        }
        if (!code.isEmpty()) {
            if (code.last(1) == "?") {
                code.removeLast();
            }
        }
    }
    C5ReplaceCharacter::replace(code);

    if (code.isEmpty()) {
        return;
    }

    ui->leCode->clear();
    ui->leCode->setFocus();

    if (!mWorkStation.scalePattern().isEmpty()) {
        if (code.startsWith(mWorkStation.scalePattern())) {
            QString weightStr = code.mid(7, 5);
            double weight = str_float(weightStr) / 1000;
            QString barcode = code.mid(2, 5);
            barcode = QString::number(barcode.toInt());
            processCode(barcode, -1, [=](const QString &c) {
                checkGoodsCode(c, [=]() {
                    ui->tblData->setCurrentCell(ui->tblData->rowCount() - 1, 0);
                    setQtyOfRow(ui->tblData->rowCount() - 1, weight);
                });
            });
            return;
        }
    }

    if (!mWorkStation.discountCardPattern().isEmpty()) {
        if (code.startsWith(mWorkStation.discountCardPattern())) {
            processCode(code, cp_t12_shop_discount, [this, code](const QString &c) { checkDiscountCardCode(c); });
            return;
        }
    }

    if (!mWorkStation.accumulateCardPatter().isEmpty()) {
        if (code.startsWith(mWorkStation.accumulateCardPatter())) {
            processAccumulateCard(code);
            return;
        }
    }

    if (!mWorkStation.presentCardPattern().isEmpty()) {
        if (code.startsWith(mWorkStation.presentCardPattern())) {
            processPresentCard(code);
            return;
        }
    }

    processCode(code, -1, [this, code](const QString &c) { checkGoodsCode(c); });
}

void WOrder::on_btnSearchPartner_clicked()
{
    QVector<PartnerItem> result = C5StructTableView::get<PartnerItem>(SelectorName<PartnerItem>::value, false, false, QPoint(-1, -1));

    if (result.isEmpty()) {
        return;
    }

    on_btnRemovePartner_clicked();
    setPartner(result.at(0));
    //TODO: make optional
    NInterface::query1("/engine/v2/common/gift-card/get-gift-of-partner-id",
                       fUser->mSessionKey,
                       this,
                       {{"id", result.at(0).id}},
                       [this](const QJsonObject &jo) {
                           if (!jo.value("code").toString().isEmpty()) {
                               if (C5Message::question(tr("Partner have a accumulate card, use?")) == QDialog::Accepted) {
                                   processAccumulateCard(jo.value("code").toString());
                               }
                           }
                       });
}

void WOrder::on_leUseAccumulated_textChanged(const QString &arg1)
{
    //TODO
    // if(arg1.toDouble() > ui->leGiftCardAmount->getDouble()) {
    //     ui->leUseAccumulated->setDouble(ui->leGiftCardAmount->getDouble());
    // }
    // if (ui->leUseAccumulated->getDouble() > ui->leTotal->getDouble()) {
    //     ui->leUseAccumulated->setDouble(ui->leTotal->getDouble());
    // }
    // if (fBHistory.type == CARD_TYPE_ACCUMULATIVE) {
    //     if (ui->leUseAccumulated->getDouble() > ui->leTotalAccumulated->getDouble()) {
    //         ui->leUseAccumulated->setDouble(ui->leTotalAccumulated->getDouble());
    //     }
    // }
}

void WOrder::on_btnRemovePartner_clicked()
{
    //TODO
    // fOHeader.partner = 0;
    // fOHeader.taxpayerTin.clear();
    // ui->leTIN->clear();
    // ui->leCustomer->clear();
    // fBHistory.card = 0;
    // fBHistory.type = 0;
    // fBHistory.value = 0;

    // for (int i = 0; i < fOGoods.count(); i++) {
    //     OGoods &og = fOGoods[i];
    //     og.discountAmount = 0;
    //     og.discountFactor = 0;
    //     og.discountMode = 0;
    // }

    // setDiscinfoVisibility(false);
}

void WOrder::processCode(const QString &code, int permission, std::function<void(const QString &)> func)
{
    auto *user = new C5User(fUser);
    auto proceed = [user, code, permission, func]() {
        if (permission == -1 || user->check(permission)) {
            func(code);
        }

        user->deleteLater();
    };

    if (permission == -1 || user->check(permission)) {
        proceed();
        return;
    }

    bool ok = false;
    const QString password = QInputDialog::getText(this, tr("Password"), tr("Password"), QLineEdit::Password, QString(), &ok);

    if (!ok || password.isEmpty()) {
        user->deleteLater();
        return;
    }

    auto http = new NInterface();
    user->authorize(
        password,
        http,
        [proceed, http](const QJsonObject &) { proceed(); },
        [user, http]() {
            user->deleteLater();
            http->deleteLater();
        });
}

void WOrder::checkDiscountCardCode(const QString &code)
{
    NInterface::query1("/engine/v2/common/discount-system/get-info",
                       fUser->mSessionKey,
                       this,
                       {{"code", code}, {"need", CARD_TYPE_DISCOUNT}},
                       [this](const QJsonObject &jdoc) {
                           QJsonObject card = jdoc["card"].toObject();
                           QJsonObject history = jdoc["history"].toObject();
                           QJsonObject partner = jdoc["partner"].toObject();
                           //TODO
                           // fBHistory.card = card["f_id"].toInt();
                           // fBHistory.type = card["f_mode"].toInt();
                           // fBHistory.value = card["f_value"].toDouble();

                           // if (fBHistory.value < 0) {
                           //     double v = fBHistory.value;

                           //     if (!getDiscountValue(CARD_TYPE_DISCOUNT, v)) {
                           //         fBHistory.card = 0;
                           //         fBHistory.type = 0;
                           //         fBHistory.value = 0;
                           //         return;
                           //     }

                           //     fBHistory.value = v;
                           // }

                           ui->leDisc->setText(QString("%1%").arg(float_str(card["f_value"].toDouble(), 2)));
                           ui->leDisc->setVisible(true);
                           ui->lbDisc->setVisible(true);
                           ui->lbDisc->setText(QString("%1: %2%").arg(tr("Discount"), float_str(card["f_value"].toDouble(), 2)));
                           ui->leCustomer->setText(partner["f_name"].toString());
                           ui->leTIN->setText(partner["f_taxcode"].toString());
                           //TODO fOHeader.partner = partner["f_id"].toInt();
                           bool discountRow = false;

                           // for (int i = 0; i < fOGoods.count(); i++) {
                           //     OGoods &og = fOGoods[i];

                           //     if (og.rowDiscount) {
                           //         discountRow = true;
                           //         break;
                           //     }
                           // }

                           // for (int i = 0; i < fOGoods.count(); i++) {
                           //     OGoods &og = fOGoods[i];

                           //     if (discountRow) {
                           //         if (og.rowDiscount) {
                           //             og.discountFactor = fBHistory.value / 100;
                           //             og.discountMode = fBHistory.type;
                           //         }
                           //     } else {
                           //         og.discountFactor = fBHistory.value / 100;
                           //         og.discountMode = fBHistory.type;
                           //     }
                           // }
                       });
}

void WOrder::checkGoodsCode(const QString &code, std::function<void()> postProcess)
{
    auto addDishProc = [&](const QJsonObject &jdoc, std::function<void()> callback) {
        const QJsonObject &jdish = jdoc.value("goods").toObject();
        QJsonObject data = jdish.value("f_data").toObject();
        data["f_barcode"] = jdish.value("f_barcode").toString();
        data["f_printed"] = true;
        data["f_adgt"] = jdish.value("f_adgt").toString();
        NInterface::query1("/engine/v2/waiter/order/add-dish",
                           fUser->mSessionKey,
                           this,
                           {{"dish", jdish.value("f_dish").toInt()},
                            {"dish_name", jdish.value("f_dish_name").toString()},
                            {"table", 1},
                            {"qty", jdish.value("f_default_qty")},
                            {"type", 1},
                            {"cashbox_id", mWorkStation.cashboxId()},
                            {"row", mOrder.dishes.count() * 100},
                            {"price", jdish.value("f_price1").toDouble()},
                            {"count_service", 0},
                            {"count_discount", 0},
                            {"f_data", data},
                            {"store", mWorkStation.defaultStoreId()},
                            {"print1", ""},
                            {"print2", ""},
                            {"empty_order", mOrder.dishes.empty()},
                            {"service_factor", 0}},
                           [=](const QJsonObject &jdoc) {
                               parseOrder(jdoc);
                               scrollOrderToBottom();
                               ui->tblData->setCurrentCell(ui->tblData->rowCount() - 1, 0);
                               if (callback) {
                                   callback();
                               }
                           });
    };

    NInterface::query1("/engine/v2/shop/process-barcode/get",
                       fUser->mSessionKey,
                       this,
                       {{"barcode", code}, {"store", mWorkStation.defaultStoreId()}, {"draft_header", ""}, {"retail", mSaleTypeMode == 1}},
                       [=](const QJsonObject &jdoc) {
                           addDishProc(jdoc, postProcess);
                           ui->leCode->setFocus();
                       });
}

void WOrder::processPresentCard(const QString &code)
{
    NInterface::query1("/engine/v2/common/gift-card/get", fUser->mSessionKey, this, {{"code", code}}, [this](const QJsonObject &jo) {
        PartnerItem pi = JsonParser<PartnerItem>::fromJson(jo.value("partner").toObject());
        setPartner(pi);
        QJsonObject jcard = jo.value("card").toObject();
        ui->lbCardValidUntil->setVisible(true);
        ui->leCardValidUntil->setVisible(true);
        ui->lbCurrentAccumlated->setVisible(true);
        ui->leCurrentAccumulated->setVisible(true);
        ui->leCurrentAccumulated->setText(jcard.value("f_sum").toString());
        ui->leCardValidUntil->setText(jcard.value("f_valid_until").toString());
        ui->leUseAccumulated->setVisible(true);
        ui->lbUseAccumulated->setVisible(true);
        ui->leUseAccumulated->setReadOnly(false);
        fGiftCard = jcard.value("f_id").toInt();
    });
}

void WOrder::processAccumulateCard(const QString &code)
{
    NInterface::query1("/engine/v2/common/gift-card/get-accumulate",
                       fUser->mSessionKey,
                       this,
                       {{"code", code}},
                       [this](const QJsonObject &jo) {
                           PartnerItem pi = JsonParser<PartnerItem>::fromJson(jo.value("partner").toObject());
                           setPartner(pi);

                           QJsonObject jcard = jo.value("card").toObject();
                           ui->leDisc->setText(QString("%1%").arg(float_str(jcard.value("f_value").toDouble(), 2)));
                           ui->leDisc->setVisible(true);
                           ui->lbDisc->setVisible(true);
                           //todo
                           // fBHistory.card = jcard.value("f_id").toInt();
                           // fBHistory.type = CARD_TYPE_ACCUMULATIVE;
                           // fBHistory.value = jcard.value("f_value").toDouble();
                           ui->lbCardValidUntil->setVisible(true);
                           ui->leCardValidUntil->setVisible(true);
                           ui->lbCurrentAccumlated->setVisible(true);
                           ui->leCurrentAccumulated->setVisible(true);
                           ui->leTotalAccumulated->setVisible(true);
                           ui->leTotalAccumulated->setText(jcard.value("f_sum").toString());
                           ui->leCardValidUntil->setText(jcard.value("f_valid_until").toString());
                           ui->leUseAccumulated->setVisible(true);
                           ui->lbUseAccumulated->setVisible(true);
                           ui->leUseAccumulated->setReadOnly(false);
                       });
}

void WOrder::printFiscal(std::function<void(const QJsonObject &)> nextStep)
{
    //TODO
    // 1. Проверка: нужно ли вообще печатать фискал
    // if (!fOHeader._printFiscal) {
    //     nextStep({});
    //     return;
    // }

    // 2. Валидация настроек (простой фискал)
    // if (__c5config.getValue(param_simple_fiscal).toInt() == 1) {
    //     if (__c5config.fMainJson["tax_dept"].toString().toInt() == 0) {
    //         C5Message::error(tr("Tax department is not set"));
    //         return;
    //     }
    // }

    // 3. Подготовка данных (Deep Copy для потока)
    struct FiscalJobData
    {
        QString ip, password, cashier, pin, extPos, partnerTin;
        int saleType, port, taxDept;
        double amountCard, amountPrepaid, amountCash;
        bool simpleFiscal;
        QList<WaiterDish> goods;
    };

    FiscalMachine fm = getFiscalMachine(mWorkStation.fiscalMachineId());
    FiscalJobData job;
    job.ip = fm.ip;
    job.port = fm.port;
    job.password = fm.machinePassword;
    //TODO WITH IDRRAM
    job.extPos = fm.externalPosString();
    job.cashier = fm.opPin;
    job.pin = fm.opPassword;
    job.partnerTin = ui->btnF4->isChecked() ? ui->leTIN->text() : "";
    job.saleType = mSaleTypeMode;
    job.amountCard = mOrder.payment(payment_fields[PAYMENT_TYPE_CARD]);
    job.amountPrepaid = mOrder.payment(payment_fields[PAYMENT_TYPE_PREPAID]);
    job.amountCash = mOrder.payment(payment_fields[PAYMENT_TYPE_CASH]);
    job.simpleFiscal = false; //todo(__c5config.getValue(param_simple_fiscal).toInt() == 1);
    job.taxDept = 1;          // todo__c5config.fMainJson["tax_dept"].toString().toInt();
    job.goods = mOrder.dishes;

    // Блокируем интерфейс
    this->setEnabled(false);
    auto *loading = new NLoadingDlg(tr("Printing fiscal check"), this);

    // Создаем поток и объект печати (БЕЗ родителя для moveToThread)
    auto *thread = new QThread();
    auto *pt = new PrintTaxN(job.ip, job.port, job.password, job.extPos, job.cashier, job.pin, nullptr);
    pt->moveToThread(thread);

    // --- ЛОГИКА В ФОНОВОМ ПОТОКЕ ---
    connect(thread, &QThread::started, pt, [=]() mutable {
        pt->fPartnerTin = job.partnerTin;

        // Если это обычная продажа (не возврат и т.д.)

        for (auto const &g : job.goods) {
            if (g.state != 1) {
                continue;
            }
            if (!g.emarks().isEmpty())
                pt->fEmarks.append(g.emarks());

            pt->addGoods(1,
                         g.adgt(),
                         QString::number(g.dishId),
                         g.fiscalName().isEmpty() ? g.dishName : g.fiscalName(),
                         g.price,
                         g.qty,
                         g.discountFactor() * 100);
        }

        if (job.simpleFiscal) {
            pt->makeJsonAndPrintSimple(job.taxDept, job.amountCard, job.amountPrepaid, "false");
        } else {
            pt->makeJsonAndPrint(job.amountCard, job.amountPrepaid);
        }

        // ВАЖНО: Методы pt выше сами вызовут emit finished(...), когда закончат работу с сетью
    });

    // --- ОБРАБОТКА РЕЗУЛЬТАТА В ГЛАВНОМ ПОТОКЕ ---
    QPointer<WOrder> self(this);
    connect(pt, &PrintTaxN::finished, this, [=](const QString &inJson, const QString &outJson, const QString &err, int result) {
        if (!self)
            return;

        self->setEnabled(true);
        loading->close();
        loading->deleteLater();

        LogWriter::write(LogWriterLevel::errors, "tt", inJson);

        QJsonObject reply{{"f_id", QUuid::createUuid().toString(QUuid::WithoutBraces)},
                          {"f_order", mOrder.id},
                          {"in", QJsonDocument::fromJson(inJson.toUtf8()).object()},
                          {"out", QJsonDocument::fromJson(outJson.toUtf8()).object()},
                          {"error", err},
                          {"result", result}};

        // Логируем попытку печати на сервер
        NInterface::query1("/engine/v2/common/fiscal/log", fUser->mSessionKey, self, reply, [](const QJsonObject &) {});

        // Обработка ошибок
        if (result != 0) {
            QString finalErr = err;
            if (finalErr.contains("-5"))
                finalErr = tr("Connection with fiscal machine lost");

            auto res = C5Message::question(finalErr, tr("Try again"), tr("Do not print fiscal"));
            if (res == QDialog::Accepted) {
                thread->quit();
                thread->wait();
                self->printFiscal(nextStep); // Рекурсивный повтор
                return;
            }
        }

        // Если всё ок, идем дальше
        if (result == 0) {
            nextStep(reply);
        }

        // Завершаем и очищаем поток
        thread->quit();
    });

    // Очистка ресурсов после завершения потока
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    connect(thread, &QThread::finished, pt, &QObject::deleteLater);

    loading->show();
    thread->start();
}

void WOrder::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    NInterface::query1("/engine/v2/waiter/order/open-table",
                       fUser->mSessionKey,
                       this,
                       {{"table", 1}, {"locksrc", hostinfo}},
                       [=](const QJsonObject &jdoc) { parseOrder(jdoc); });
}

void WOrder::setDiscinfoVisibility(bool v)
{
    ui->lbDisc->setVisible(v);
    ui->leDisc->setVisible(v);
    ui->lbUseAccumulated->setVisible(v);
    ui->leUseAccumulated->setVisible(v);
    ui->leCurrentAccumulated->setVisible(v);
    ui->lbCurrentAccumlated->setVisible(v);
    ui->lbCardValidUntil->setVisible(v);
    ui->leCardValidUntil->setVisible(v);
    ui->leTotalAccumulated->setVisible(v);
}

void WOrder::on_btnAddPartner_clicked()
{
    DlgShopCustomer d(fUser);
    d.setTin(ui->leTIN->text());
    if (d.exec() == QDialog::Accepted) {
        setPartner(d.mPartner);
    }
}

void WOrder::parseOrder(const QJsonObject &jdoc)
{
    ui->tblData->setColumnDecimals(col_qty, 3);
    mOrder = JsonParser<WaiterOrder>::fromJson(jdoc["order"].toObject());
    ui->tblData->setRowCount(mOrder.normalDishesCount());
    int i = 0;
    for (auto const &g : mOrder.dishes) {
        if (g.state != 1) {
            continue;
        }
        ui->tblData->setData(i, 0, QVariant::fromValue(g));
        ui->tblData->setString(i, col_barcode, g.barcode());
        ui->tblData->setString(i, col_name, g.dishName);
        ui->tblData->setDouble(i, col_qty, g.qty);
        ui->tblData->setDouble(i, col_price, g.price);
        ui->tblData->setString(i, col_unit, g.unitName);
        ui->tblData->setDouble(i, col_total, g.total(false));
        ui->tblData->setRowHidden(i, g.state != 1);
        i++;
    }
    ui->leTotal->setDouble(mOrder.totalDue);
}

void WOrder::scrollOrderToBottom()
{
    ui->tblData->scrollToBottom();
}

void CustomDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // 1. Проверяем наличие цвета в BackgroundRole
    QVariant bgData = index.data(Qt::BackgroundRole);

    if (bgData.isValid()) {
        QColor customColor = bgData.value<QColor>();

        // Сохраняем состояние рисовальщика
        painter->save();

        // 2. Рисуем наш кастомный фон принудительно по всей области ячейки
        painter->fillRect(opt.rect, customColor);

        // 3. Чтобы системное выделение не перекрыло наш цвет,
        // убираем флаг "Selected" из опций отрисовки фона
        opt.state &= ~QStyle::State_Selected;

        // 4. Отрисовываем всё остальное (текст, иконки) поверх нашего фона
        QStyledItemDelegate::paint(painter, opt, index);

        // 5. Если строка выделена, можно нарисовать тонкую рамку,
        // чтобы пользователь понимал, где курсор
        if (option.state & QStyle::State_Selected) {
            painter->setPen(option.palette.color(QPalette::Highlight));
            painter->drawRect(opt.rect.adjusted(0, 0, -1, -1));
        }

        painter->restore();
    } else {
        // Если цвета нет — рисуем как обычно
        QStyledItemDelegate::paint(painter, option, index);
    }
    QStyledItemDelegate::paint(painter, opt, index);
}
