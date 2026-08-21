#include "worder.h"
#include <QDate>
#include <QDir>
#include <QFile>
#include <QInputDialog>
#include <QPainter>
#include <QPointer>
#include <QPrinter>
#include <QPrinterInfo>
#include <QScrollBar>
#include <QSettings>
#include <QThread>
#include <QTime>
#include <cmath>
#include "c5checkbox.h"
#include "c5lineedit.h"
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
#include "format_date.h"
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
#include "dict_dish_state.h"
#include "ui_worder.h"
#include "wcustomerdisplay.h"
#include "working.h"

WOrder::WOrder(C5User *user, int saleType, WCustomerDisplay *customerDisplay, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WOrder)
{
    ui->setupUi(this);
    ui->leTotal->setText("0");

    fUser = user;
    fWorking = static_cast<Working *>(parent);
    Q_UNUSED(customerDisplay);
    hideLoyaltyPanels();
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

void WOrder::setTableId(int tableId)
{
    mTableId = tableId > 0 ? tableId : 1;
}

void WOrder::loadExistingOrder(const QJsonObject &jdoc, int tableId)
{
    mSkipOpenTableOnShow = true;
    setTableId(tableId);
    parseOrder(jdoc);
}

void WOrder::updateCustomerDisplay()
{
    Working *working = fWorking ? fWorking : Working::working();
    if (!working) {
        return;
    }
    // Only the active sale tab paints the buyer screen.
    if (working->worder() != this) {
        return;
    }
    WCustomerDisplay *cd = working->customerDisplay();
    if (!cd) {
        return;
    }

    cd->clear();
    int painted = 0;
    for (int i = 0; i < mOrder.dishes.size(); ++i) {
        const auto &g = mOrder.dishes.at(i);
        // Same filter as the sale table (state == 1 / OK).
        if (g.state != 1) {
            continue;
        }
        cd->addRow(g.dishName,
                    float_str(g.qty, 2),
                    float_str(g.price, 2),
                    float_str(g.total(false), 2),
                    float_str(g.discountFactor(), 2));
        ++painted;
    }
    // Fallback: if server sent lines without state yet, still show something.
    if (painted == 0) {
        for (int i = 0; i < mOrder.dishes.size(); ++i) {
            const auto &g = mOrder.dishes.at(i);
            if (g.qty <= 0) {
                continue;
            }
            cd->addRow(g.dishName,
                        float_str(g.qty, 2),
                        float_str(g.price, 2),
                        float_str(g.total(false), 2),
                        float_str(g.discountFactor(), 2));
        }
    }
    const QString totalText = ui->leTotal->text().trimmed().isEmpty()
                                  ? float_str(mOrder.totalDue, 2)
                                  : ui->leTotal->text();
    cd->setTotal(totalText);
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
    Q_UNUSED(nextStep);
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
    double prepaidFromCard = 0;
    double maxPrepaid = 0;

    if (mLoyalty.hasGift() || mLoyalty.hasAccumulate()) {
        prepaidFromCard = mLoyalty.useAmount;
        maxPrepaid = qMin(mLoyalty.activeBalance(), mOrder.totalDue);
        prepaidReadonly = prepaidFromCard > 0;
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

    // After F12: optionally choose seller, then payment and registration.
    QPointer<WOrder> self(this);
    auto continuePayment = [=]() {
        if (!self) {
            return;
        }

        bool _printFiscal = true; //TODO__c5config.alwaysOfferTaxPrint() || ui->btnF4->isChecked();

        // F12: payment dialog defaults to cash (remainder after prepaid).
        // Card / debt / etc. are chosen in the dialog if needed.
        double prepaid = prepaidFromCard;
        double cash = self->mOrder.totalDue - prepaid;
        if (cash < 0) {
            cash = 0;
        }
        double card = 0, idram = 0, bank = 0, telcell = 0, debt = 0, cashin = 0, change = 0;
        QJsonObject pinpadResponse;
        if (!DlgPaymentChoose::getValues(
                self->fUser, self->mOrder.totalDue, cash, card, idram, telcell, bank, prepaid, debt, cashin, change, _printFiscal, prepaidReadonly, maxPrepaid, &pinpadResponse)) {
            return;
        }
        self->mLoyalty.useAmount = prepaid;

        // Для долга и банковского перевода нужен партнёр (ИИН не обязателен).
        if ((debt > 0.001 || bank > 0.001) && self->mPartnerId <= 0) {
            C5Message::error(tr("A partner must be selected for debt / bank transfer"));
            return;
        }

        FiscalMachine fm = getFiscalMachine(mWorkStation.fiscalMachineId());

        if (fm.id == 0) {
            _printFiscal = false;
        }

        const int cashSessionId = Working::working()->cashSessionId();
        if (cashSessionId <= 0) {
            C5Message::error(tr("Cashbox session is not open"));
            return;
        }

        QJsonObject setAmountsParams{
            {QStringLiteral("id"), self->mOrder.id},
            {QStringLiteral("f_amount_cash"), cash},
            {QStringLiteral("f_amount_card"), card},
            {QStringLiteral("f_amount_bank"), bank},
            {QStringLiteral("f_amount_idram"), idram},
            {QStringLiteral("f_amount_debt"), debt},
            {QStringLiteral("f_amount_telcell"), telcell},
            {QStringLiteral("f_amount_prepaid"), prepaid}};
        if (!pinpadResponse.isEmpty()) {
            setAmountsParams.insert(QStringLiteral("f_pinpad_response"), pinpadResponse);
        }

        auto startPayment = [=]() {
            if (!self) {
                return;
            }
            // Повторная проверка после синхронизации f_guest на сервер.
            if ((debt > 0.001 || bank > 0.001) && self->mPartnerId <= 0) {
                C5Message::error(tr("A partner must be selected for debt / bank transfer"));
                return;
            }

            NInterface::query1("/engine/v2/waiter/order/set-amounts",
                               self->fUser->mSessionKey,
                               self,
                               setAmountsParams,
                               [=](const QJsonObject &jdoc) {
                                   if (!self) {
                                       return;
                                   }
                                   self->parseOrder(jdoc);

                                   auto afterServicePrint = [=]() {
                                       auto closeOrderFunc = [=](const QJsonObject &fiscalInfo) {
                                           if (!self) {
                                               return;
                                           }

                                           NInterface::query1("/engine/v2/waiter/order/close-order",
                                                              self->fUser->mSessionKey,
                                                              self,
                                                              {{"id", self->mOrder.id},
                                                               {"fiscal", fiscalInfo},
                                                               {"cashbox_id", mWorkStation.cashboxId()},
                                                               {"cash_session_id", cashSessionId},
                                                               {"staff_id", self->mStaffId},
                                                               {"loyalty", self->loyaltyPayload()}},
                                                              [self](const QJsonObject &jdoc) {
                                                                  self->parseOrder(jdoc);
                                                                  self->printPrecheck();
                                                                  self->mLoyalty.clear();
                                                                  self->hideLoyaltyPanels();
                                                                  emit self->orderSaved(self);
                                                              });
                                       };

                                       if (_printFiscal) {
                                           self->printFiscal([=](const QJsonObject &jtax) { closeOrderFunc(jtax); });
                                       } else {
                                           closeOrderFunc({});
                                       }
                                   };

                                   if (self->needsServicePrint()) {
                                       NInterface::query1("/engine/v2/waiter/order/print-service-check",
                                                          self->fUser->mSessionKey,
                                                          self,
                                                          {{"header_id", self->mOrder.id},
                                                           {"reprint", true}},
                                                          [=](const QJsonObject &ps) {
                                                              if (!self) {
                                                                  return;
                                                              }
                                                              self->printServiceCheck(ps);
                                                              self->parseOrder(ps);
                                                              afterServicePrint();
                                                          });
                                   } else {
                                       afterServicePrint();
                                   }
                               });
        };

        // Перед оплатой убеждаемся, что f_guest уже на заказе (партнёр мог быть выбран до первого товара).
        if (self->mPartnerId > 0) {
            self->persistGuestToOrder(startPayment);
        } else {
            startPayment();
        }
    };

    if (mWorkStation.assignSaleToAssociate()) {
        fWorking->loadStaff([=]() {
            if (!self) {
                return;
            }
            int staffId = 0;
            if (!SelectStaff::select(self->fWorking, self->fUser, staffId)) {
                return;
            }
            self->mStaffId = staffId;
            continuePayment();
        });
    } else {
        mStaffId = fUser->id();
        continuePayment();
    }
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

    //TODO
    // auto &g = dish from row;
    // if (__c5config.shopDenyPriceChange() && !(g.price < 0)) {
    //     return;
    // }

    double price = DQty::getQty(tr("Price"), 0, this);

    if (price < 0) {
        return;
    }

    setPriceOfRow(row, price);
}

void WOrder::changePrice(double price)
{
    int row = ui->tblData->currentRow();

    if (row < 0) {
        return;
    }

    setPriceOfRow(row, price);
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

bool WOrder::needsServicePrint() const
{
    for (const auto &d : mOrder.dishes) {
        if (d.state != 1) {
            continue;
        }
        if (!d.printer1().trimmed().isEmpty() || !d.printer2().trimmed().isEmpty()) {
            return true;
        }
    }
    return false;
}

void WOrder::printServiceCheck(const QJsonObject &jdoc)
{
    QJsonObject printData = jdoc.value(QStringLiteral("print_data")).toObject();
    const QStringList printers = printData.keys();
    QJsonObject jh = jdoc.value(QStringLiteral("header")).toObject();
    if (printers.isEmpty()) {
        return;
    }

    for (const QString &printerName : printers) {
        QJsonObject jo = printData.value(printerName).toObject();
        QFont font(qApp->font());
        const int bs = 22;
        font.setPointSize(bs);
        C5Printing p;
        QPrinterInfo pi = QPrinterInfo::printerInfo(printerName);
        QPrinter printer(pi);
        QRectF pr = printer.pageRect(QPrinter::DevicePixel);
        p.setSceneParams(pr.width(), pr.height(), 96.0);
        p.setFont(font);
        p.setFontBold(true);
        p.setFontSize(bs);
        constexpr int sideMarginMm = 5;
        p.setRightMarginMm(sideMarginMm);
        const int nameWidthMm = qMax(8, 65 - 2 * sideMarginMm);

        p.ctext(tr("New order").toUpper());
        p.br();
        p.br();
        p.setFontBold(false);
        p.ltext(tr("Order no"), sideMarginMm);
        p.rtext(jh.value(QStringLiteral("f_prefix")).toString());
        p.br();
        p.ltext(tr("Date"), sideMarginMm);
        p.rtext(QDate::currentDate().toString(FORMAT_DATE_TO_STR));
        p.br();
        p.ltext(tr("Time"), sideMarginMm);
        p.rtext(QTime::currentTime().toString(FORMAT_TIME_TO_STR));
        p.br();
        p.ltext(tr("Staff"), sideMarginMm);
        p.rtext(fUser->shortFullName());
        p.br();
        p.line();
        p.br(2);

        for (const auto &jdv : jo.value(QStringLiteral("dishes")).toArray()) {
            p.setFontSize(bs + 2);
            p.setFontBold(false);
            QJsonObject jd = jdv.toObject();
            p.ltext(jd.value(QStringLiteral("f_dish_name")).toString(), sideMarginMm, nameWidthMm);
            p.setFontBold(true);
            p.rtext(float_str(jd.value(QStringLiteral("f_qty")).toDouble(), 2));
            if (!jd.value(QStringLiteral("f_comment")).toString().isEmpty()) {
                p.br();
                p.setFontSize(bs - 4);
                p.setFontBold(true);
                p.ltext(jd.value(QStringLiteral("f_comment")).toString(), sideMarginMm, 650);
                p.br();
                p.setFontSize(bs + 2);
            }
            p.br();
            p.line();
            p.br(1);
        }

        p.line();
        p.br(1);
        p.setFontSize(bs - 6);
        p.ltext(QString("%1 %2").arg(tr("Printer: "), printerName), sideMarginMm);
        p.setFontBold(true);
        p.rtext(jo.value(QStringLiteral("side")).toString());
        p.br();

        if (!pi.isNull()) {
            if (!p.print(printer)) {
                C5Message::error(tr("Print error"));
            }
        } else {
            C5Message::error(tr("Printer not found") + ": " + printerName);
        }
    }
}

void WOrder::printPrecheck()
{
    if (!mWorkStation.isReceiptPrintingConfigured()) {
        return;
    }
    int bs = mWorkStation.data.value("f_precheck_font_size").toInt();
    if (bs == 0) {
        bs = 16;
    }
    QFont font(qApp->font());
    font.setPointSize(bs);
    const QString printerName = mWorkStation.receiptPrinter();
    C5Printing p;

    // Layout metrics: receipt_printer when set; for print_server-only fall back to fixed width.
    QPrinter *printer = nullptr;
    QPrinterInfo pi;
    if (mWorkStation.hasReceiptPrinter()) {
        pi = QPrinterInfo::printerInfo(printerName);
    }
    QPrinter localPrinter(pi.isNull() ? QPrinterInfo() : pi);
    if (!pi.isNull()) {
        printer = &localPrinter;
        localPrinter.setPageSize(QPageSize::Custom);
        localPrinter.setFullPage(false);
        QRectF pr = localPrinter.pageRect(QPrinter::DevicePixel);
        constexpr qreal SAFE_RIGHT_MM = 4.0;
        qreal safePx = SAFE_RIGHT_MM * localPrinter.logicalDpiX() / 25.4;
        p.setSceneParams(pr.width() - safePx, pr.height(), localPrinter.logicalDpiX());
    } else {
        p.setSceneParams(650, 2800, 96);
    }
    p.setFont(font);
    p.setFontSize(bs);
    const int marginMm = qMax(0, mWorkStation.data.value(QStringLiteral("f_precheck_margins")).toInt());
    p.setRightMarginMm(marginMm);
    const int colQty = 33 + marginMm;
    const int colPrice = 41 + marginMm;
    const int nameWidthMm = qMax(10, 35 - marginMm);
    QString logoFile = qApp->applicationDirPath() + "/logo_receipt.png";

    if (QFile::exists(logoFile)) {
        p.image(logoFile, Qt::AlignHCenter);
        p.br();
    }

    switch (mOrder.state) {
    case ORDER_STATE_OPEN:
    case ORDER_STATE_CLOSE:
        p.ltext(tr("Receipt"), marginMm);
        break;

    case ORDER_STATE_PREORDER:
        p.ltext(tr("Preorder"), marginMm);
        break;

    default:
        p.ltext(QString::number(mOrder.state) + ": " + tr("Error in state"), marginMm);
        break;
    }

    p.rtext(mOrder.receiptNumber);
    p.br();
    QJsonObject jtax = mOrder.fiscal();

    if (!jtax.isEmpty()) {
        // jtax = QJsonDocument::fromJson(jtax.value("out").toString().toUtf8()).object();
        p.ltext(jtax["taxpayer"].toString(), marginMm);
        p.br();
        p.ltext(jtax["address"].toString(), marginMm);
        p.br();
        p.ltext(tr("TIN"), marginMm);
        p.rtext(jtax["tin"].toString());
        p.br();
        p.ltext(tr("Device number"), marginMm);
        p.rtext(jtax["crn"].toString());
        p.br();
        p.ltext(tr("Serial"), marginMm);
        p.rtext(jtax["sn"].toString());
        p.br();
        p.ltext(tr("Fiscal"), marginMm);
        p.rtext(jtax["fiscal"].toString());
        p.br();
        p.ltext(tr("Receipt number"), marginMm);
        p.rtext(QString::number(jtax["rseq"].toInt()));
        p.br();
        p.ltext(tr("Date"), marginMm);
        p.rtext(QDateTime::fromMSecsSinceEpoch(jtax["time"].toDouble()).toString(FORMAT_DATETIME_TO_STR));
        p.br();
        p.ltext(tr("(F)"), marginMm);
        p.br();
    }

    p.br(1);
    p.ltext(tr("Table"), marginMm);
    p.rtext(QString("%1/%2").arg(mOrder.hallName, mOrder.tableName));
    p.br();
    p.line(2);
    p.br(2);
    //p.setFontSize(bs - 4);
    p.ltext(tr("Name"), marginMm);
    p.ltext(tr("Qty"), colQty);
    p.ltext(tr("Price"), colPrice);
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
            p.ltext(QString("%1: %2").arg(tr("Class"), dish.adgtCode()), marginMm);
            p.br();
        }

        QString name = dish.translated();

        p.ltext(name, marginMm, nameWidthMm);
        p.ltext(float_str(dish.qty, 2), colQty, 8);
        p.ltext(float_str(dish.price, 2), colPrice, 12);
        p.rtext(float_str(dish.total(mOrder.state == ORDER_STATE_PREORDER), 2));
        p.br();
        p.br(2);
        p.line();
        p.br(1);
    }

    p.setFontSize(bs - 2);

    p.setFontSize(bs + 2);
    p.ltext(tr("Subtotal"), marginMm);
    p.rtext(float_str(mOrder.subTotal(), 2));
    p.br();

    if (mOrder.serviceFactor() > 0) {
        //p.ltext(QString("%1 %2%").arg(tr("Service")).arg(mOrder.serviceFactor() * 100), 0);
        //p.rtext(float_str(mOrder.serviceAmount(), 2));
        p.ltext(QString("%1").arg(tr("Service")), marginMm);
        p.rtext("+" + float_str(mOrder.serviceFactor() * 100, 2) + "%");
        p.br();
    }

    if (mOrder.discountFactor() > 0) {
        // p.ltext(QString("%1 %2%").arg(tr("Discount"), float_str(mOrder.discountFactor() * 100, 2)), 0);
        // p.rtext(float_str(mOrder.discountAmount(), 2));
        p.ltext(QString("%1").arg(tr("Discount")), marginMm);
        p.rtext("-" + float_str(mOrder.discountFactor() * 100, 2) + "%");
        p.br();
    }

    if (mOrder.prepaidAmount() > 0) {
        p.ltext(tr("Prepaid amount"), marginMm);
        p.setFontSize(bs);
        p.rtext(float_str(mOrder.prepaidAmount() * -1, 2));
        p.br();
    }

    p.ltext(tr("Total due"), marginMm);
    p.rtext(float_str(mOrder.totalDue, 2));
    p.br();
    p.br();
    auto printPaymentFunc = [this, &p, marginMm](int id) {
        if (mOrder.payment(payment_fields[id]) > 0) {
            p.ltext(QCoreApplication::translate("PaymentType", payment_names[id]), marginMm);
            p.rtext(float_str(mOrder.payment(payment_fields[id]), 2));
            p.br();
        }
    };

    for (auto pt : payment_types) {
        printPaymentFunc(pt);
    }

    if (mOrder.amountPaid() - mOrder.totalDue > 0) {
        p.br();
        p.ltext(tr("Amount paid"), marginMm);
        p.rtext(float_str(mOrder.amountPaid(), 2));
        p.br();
        p.ltext(tr("Change"), marginMm);
        p.rtext(float_str(mOrder.amountPaid() - mOrder.totalDue, 2));
        p.br();
    }

    p.br();
    p.setFontSize(bs - 2);
    p.ltext(tr("Thank you for visit!"), marginMm);
    p.br();

    if (mOrder.state == ORDER_STATE_OPEN || mOrder.state == ORDER_STATE_CLOSE) {
        p.ltext(QString("%1: %2").arg(tr("Sample")).arg(mOrder.printCount()), marginMm);
    }

    p.br();
    p.ltext(tr("Printed"), marginMm);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();

    // 1) print_server set → HTTP; 2) receipt_printer set → local Qt; neither → already skipped above
    if (mWorkStation.usePrintServer()) {
        HttpLite *http = new HttpLite(this);
        QJsonObject json;
        json["print_data"] = p.jsonData();
        json["printer_name"] = printerName;
        http->post(mWorkStation.printServer(), json);
    }
    if (mWorkStation.hasReceiptPrinter()) {
        if (printer) {
            p.print(*printer);
        } else {
            C5Message::error(tr("Printer not found") + ": " + printerName);
        }
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
    mPartnerId = pi.id;
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
    ui->btnF5->setChecked(mWorkStation.quickDebtPartnerId() == pi.id);

    // ИИН не обязателен: для долга/банка нужен только f_guest_id на заказе.
    // Если заказ ещё не создан — сохраним партнёра при первом parseOrder с id.
    persistGuestToOrder();
}

void WOrder::persistGuestToOrder(std::function<void()> nextStep)
{
    if (mOrder.id.isEmpty() || mPartnerId <= 0) {
        if (nextStep) {
            nextStep();
        }
        return;
    }

    const QJsonObject guest{{QStringLiteral("f_guest_id"), mPartnerId},
                            {QStringLiteral("f_guest_tin"), ui->leTIN->text().trimmed()},
                            {QStringLiteral("f_guest_name"), ui->leCustomer->text().trimmed()}};
    NInterface::query1(QStringLiteral("/engine/v2/waiter/order/set-data-value"),
                       fUser->mSessionKey,
                       this,
                       {{QStringLiteral("id"), mOrder.id},
                        {QStringLiteral("key"), QStringLiteral("f_guest")},
                        {QStringLiteral("value"), guest}},
                       [this, nextStep](const QJsonObject &jdoc) {
                           parseOrder(jdoc);
                           if (nextStep) {
                               nextStep();
                           }
                       });
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
    } else {
        // Weight / scale: at most 3 decimals (truncate, do not round).
        qty = std::floor(qty * 1000.0 + 1e-9) / 1000.0;
    }

    if (!allowStockForDish(og.dishId, og.isService(), og.dataValue(QStringLiteral("f_stock")).toDouble(), qty, og.id)) {
        return false;
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
    auto const &og = ui->tblData->getData(row, 0).value<WaiterDish>();
    if (og.id.isEmpty()) {
        C5Message::error("Error og.id.isEmpty == true");
        return false;
    }

    NInterface::query1("/engine/v2/waiter/order/set-dish-price",
                       fUser->mSessionKey,
                       this,
                       {{"id", og.id},
                        {"dish", og.dishId},
                        {"dish_name", og.dishName},
                        {"new_price", price},
                        {"order_id", mOrder.id}},
                       [=](const QJsonObject &jdoc) { parseOrder(jdoc); });

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

    // Piece scale label EAN-13 (hardcoded): 24[5 PLU][5 qty][check]
    // Example 2401035000023 → flag 24, scancode 01035 → 1035, qty 00002 → 2
    if (code.length() == 13 && code.startsWith(QStringLiteral("24"))) {
        const QString pluRaw = code.mid(2, 5);
        QString barcode = QString::number(pluRaw.toInt());
        if (barcode == QLatin1String("0") && pluRaw == QLatin1String("00000")) {
            barcode.clear();
        }
        const int qty = code.mid(7, 5).toInt();
        if (!barcode.isEmpty() && qty > 0) {
            processCode(barcode, -1, [=](const QString &c) {
                checkGoodsCode(c, [=]() {
                    ui->tblData->setCurrentCell(ui->tblData->rowCount() - 1, 0);
                    setQtyOfRow(ui->tblData->rowCount() - 1, qty);
                });
            });
            return;
        }
    }

    // Scale label EAN: [2-digit flag][5-digit PLU][6-digit weight*10000]
    // Example 2301001002405 → flag 23, scancode 01001 → 1001, weight 002405 → 0.2405 kg
    // Match configured pattern, or any EAN-13 in the 20–29 weighted range (except 24 = piece labels above).
    // Config often has 22 while labels use 23.
    {
        const QString pattern = mWorkStation.scalePattern().trimmed();
        int prefixLen = 0;
        bool isScale = false;

        if (!pattern.isEmpty()
            && pattern != QLatin1String("24")
            && code.startsWith(pattern)
            && code.length() >= pattern.length() + 11) {
            prefixLen = pattern.length();
            isScale = true;
        } else if (code.length() == 13
                   && code.at(0) == QLatin1Char('2')
                   && code.at(1).isDigit()
                   && !code.startsWith(QStringLiteral("24"))) {
            prefixLen = 2;
            isScale = true;
        }

        if (isScale) {
            const QString pluRaw = code.mid(prefixLen, 5);
            QString barcode = QString::number(pluRaw.toInt());
            if (barcode == QLatin1String("0") && pluRaw == QLatin1String("00000")) {
                barcode.clear();
            }
            const QString weightStr = code.mid(prefixLen + 5, 6);
            const double weight = str_float(weightStr) / 10000.0;

            if (!barcode.isEmpty() && weight > 0) {
                processCode(barcode, -1, [=](const QString &c) {
                    checkGoodsCode(c, [=]() {
                        ui->tblData->setCurrentCell(ui->tblData->rowCount() - 1, 0);
                        setQtyOfRow(ui->tblData->rowCount() - 1, weight);
                    });
                });
                return;
            }
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

void WOrder::on_leUseAccumulated_3_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    clampUseAmount(ui->leUseAccumulated_3);
}

void WOrder::on_leUseAccumulated_4_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    clampUseAmount(ui->leUseAccumulated_4);
}

void WOrder::on_btnRemovePartner_clicked()
{
    ui->leTIN->clear();
    ui->leCustomer->clear();
    ui->btnF5->setChecked(false);
    mPartnerId = 0;

    const bool hadDiscount = mLoyalty.hasDiscount();
    mLoyalty.clear();
    hideLoyaltyPanels();

    // Снимаем ранее применённую скидку с заказа на сервере.
    if (hadDiscount && !mOrder.id.isEmpty()) {
        NInterface::query1("/engine/v2/waiter/order/change-discount-value",
                           fUser->mSessionKey,
                           this,
                           {{"id", mOrder.id}, {"value", 0}, {"comment", "remove"}},
                           [this](const QJsonObject &jo) { parseOrder(jo); });
    }

    if (!mOrder.id.isEmpty()) {
        NInterface::query1("/engine/v2/waiter/order/set-data-value",
                           fUser->mSessionKey,
                           this,
                           {{"id", mOrder.id},
                            {"key", "f_guest"},
                            {"value", QJsonObject()}},
                           [this](const QJsonObject &jdoc) { parseOrder(jdoc); });
    }
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
                           QJsonObject partner = jdoc["partner"].toObject();

                           const double value = card["f_value"].toDouble();
                           mLoyalty.discountCardId = card["f_id"].toInt();
                           mLoyalty.discountType = card["f_mode"].toInt();
                           mLoyalty.discountFactor = value;
                           mLoyalty.discountPartnerId = partner["f_id"].toInt();

                           ui->leDisc_2->setText(QString("%1%").arg(float_str(value, 2)));
                           ui->lbDisc_2->setText(QString("%1: %2%").arg(tr("Discount"), float_str(value, 2)));
                           ui->wDiscount->setVisible(true);

                           PartnerItem pi = JsonParser<PartnerItem>::fromJson(partner);
                           setPartner(pi);

                           // Применяем скидку к заказу на сервере и обновляем итоги.
                           NInterface::query1("/engine/v2/waiter/order/change-discount-value",
                                              fUser->mSessionKey,
                                              this,
                                              {{"id", mOrder.id},
                                               {"value", value / 100.0},
                                               {"comment", QString("card %1").arg(mLoyalty.discountCardId)}},
                                              [this](const QJsonObject &jo) { parseOrder(jo); });
                       });
}

void WOrder::checkGoodsCode(const QString &code, std::function<void()> postProcess)
{
    checkGoodsId(0, code, postProcess);
}

void WOrder::checkGoodsId(int goodsId, const QString &scancode, std::function<void()> postProcess, double knownStock)
{
    auto addDishProc = [this, knownStock](const QJsonObject &jdoc, std::function<void()> callback) {
        const QJsonObject &jdish = jdoc.value("goods").toObject();
        const QJsonObject store = jdoc.value("store").toObject();
        QJsonObject data = jdish.value("f_data").toObject();
        const QString servicePrint = data.value(QStringLiteral("f_service_print")).toString().trimmed();
        data["f_barcode"] = jdish.value("f_barcode").toString();
        // Totals need f_printed=true; kitchen paper is printed at sale close with reprint.
        data["f_printed"] = true;
        data["f_adgt"] = jdish.value("f_adgt").toString();
        data["f_groupname"] = jdish.value("f_groupname").toString();
        data["f_qtybox"] = jdish.value("f_qtybox").toDouble();
        data["f_is_service"] = jdish.value("f_is_service").toInt() > 0;
        double stockQty = store.contains(QStringLiteral("f_qty"))
                              ? store.value(QStringLiteral("f_qty")).toDouble()
                              : jdish.value(QStringLiteral("f_stock")).toDouble(
                                    jdish.value(QStringLiteral("f_qty")).toDouble());
        // Qty from F5 goods list (same source as the list UI)
        if (knownStock >= 0) {
            stockQty = knownStock;
        }
        data["f_stock"] = stockQty;
        const int dishId = jdish.value("f_dish").toInt();
        const bool isService = jdish.value("f_is_service").toInt() > 0;
        const double addQty = jdish.value("f_default_qty").toDouble(1);
        if (!allowStockForDish(dishId, isService, stockQty, addQty)) {
            return;
        }
        NInterface::query1("/engine/v2/waiter/order/add-dish",
                           fUser->mSessionKey,
                           this,
                           {{"dish", dishId},
                            {"dish_name", jdish.value("f_dish_name").toString()},
                            {"table", tableId()},
                            {"qty", addQty},
                            {"type", 1},
                            {"cashbox_id", mWorkStation.cashboxId()},
                            {"row", mOrder.dishes.count() * 100},
                            {"price", jdish.value("f_price1").toDouble()},
                            {"count_service", 0},
                            {"count_discount", 0},
                            {"f_data", data},
                            {"store", mWorkStation.defaultStoreId()},
                            {"print1", servicePrint},
                            {"print2", ""},
                            {"create_process", !servicePrint.isEmpty()},
                            {"shop_mode", 1},
                            {"empty_order", mOrder.dishes.empty()},
                            {"staff_id", mStaffId},
                            {"service_factor", 0}},
                           [=](const QJsonObject &jdoc) {
                               parseOrder(jdoc);
                               scrollOrderToBottom();
                               ui->tblData->setCurrentCell(ui->tblData->rowCount() - 1, 0);
                               // Comment typed before first item — persist once order exists
                               if (!mOrder.id.isEmpty()
                                   && ui->leComment->text().trimmed() != mOrder.comment()) {
                                   on_leComment_editingFinished();
                               }
                               // Partner selected before first item (TIN may be empty) — write f_guest
                               if (mPartnerId > 0
                                   && mOrder.data.value(QStringLiteral("f_guest"))
                                          .toObject()
                                          .value(QStringLiteral("f_guest_id"))
                                          .toInt()
                                          <= 0) {
                                   persistGuestToOrder();
                               }
                               if (callback) {
                                   callback();
                               }
                           });
    };

    QJsonObject params{{"barcode", scancode},
                       {"store", mWorkStation.defaultStoreId()},
                       {"draft_header", ""},
                       {"retail", mSaleTypeMode == 1}};
    if(goodsId > 0) {
        params.insert(QStringLiteral("goods_id"), goodsId);
    }

    NInterface::query1("/engine/v2/shop/process-barcode/get",
                       fUser->mSessionKey,
                       this,
                       params,
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

        // Подарочная и накопительная взаимоисключающие.
        mLoyalty.clearAccumulate();
        ui->wAccumulateCard->setVisible(false);

        mLoyalty.giftCardId = jcard.value("f_id").toInt();
        mLoyalty.giftBalance = str_float(jcard.value("f_sum").toString());
        mLoyalty.useAmount = 0;

        ui->leCurrentAccumulated_3->setText(jcard.value("f_sum").toString());
        ui->leCardValidUntil_3->setText(jcard.value("f_valid_until").toString());
        ui->leUseAccumulated_3->setReadOnly(false);
        ui->leUseAccumulated_3->setText("0");
        ui->wGiftCard->setVisible(true);
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

                           // Накопительная и подарочная взаимоисключающие.
                           mLoyalty.clearGift();
                           ui->wGiftCard->setVisible(false);

                           mLoyalty.accumulateCardId = jcard.value("f_id").toInt();
                           mLoyalty.accumulateBalance = str_float(jcard.value("f_sum").toString());
                           mLoyalty.accumulatePercent = jcard.value("f_value").toDouble();
                           mLoyalty.useAmount = 0;

                           ui->leDisc_4->setText(QString("%1%").arg(float_str(mLoyalty.accumulatePercent, 2)));
                           ui->lbDisc_4->setText(QString("%1: %2%").arg(tr("Accumulate"), float_str(mLoyalty.accumulatePercent, 2)));
                           ui->leCurrentAccumulated_4->setText(jcard.value("f_sum").toString());
                           ui->leTotalAccumulated_4->setText(jcard.value("f_sum").toString());
                           ui->leCardValidUntil_4->setText(jcard.value("f_valid_until").toString());
                           ui->leUseAccumulated_4->setReadOnly(false);
                           ui->leUseAccumulated_4->setText("0");
                           ui->wAccumulateCard->setVisible(true);
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
        int saleType, port, taxDept, paymentSystem;
        double amountNonCash, amountPrepaid, amountCash;
        bool forceInternalPos;
        bool simpleFiscal;
        QList<WaiterDish> goods;
    };

    FiscalMachine fm = getFiscalMachine(mWorkStation.fiscalMachineId());
    FiscalJobData job;
    job.ip = fm.ip;
    job.port = fm.port;
    job.password = fm.machinePassword;
    job.extPos = fm.externalPosString();
    job.cashier = fm.opPin;
    job.pin = fm.opPassword;
    job.partnerTin = ui->btnF4->isChecked() ? ui->leTIN->text() : "";
    job.saleType = mSaleTypeMode;
    const double card = mOrder.payment(payment_fields[PAYMENT_TYPE_CARD]);
    const double idram = mOrder.payment(payment_fields[PAYMENT_TYPE_IDRAM]);
    const double telcell = mOrder.payment(payment_fields[PAYMENT_TYPE_TELCELL]);
    job.amountNonCash = card + idram + telcell;
    job.amountPrepaid = mOrder.payment(payment_fields[PAYMENT_TYPE_PREPAID]);
    job.amountCash = mOrder.payment(payment_fields[PAYMENT_TYPE_CASH]);
    job.paymentSystem = -1;
    job.forceInternalPos = false;
    // HDM v0.7.3 PaymentSystem: bank card=1, Telcell=10, Idram=13.
    // Only one PaymentSystem can be sent, so use the dominant non-cash payment.
    if (card >= idram && card >= telcell && card > 0.001) {
        job.paymentSystem = 1;
    } else if (idram >= telcell && idram > 0.001) {
        job.paymentSystem = 13;
        job.forceInternalPos = true;
    } else if (telcell > 0.001) {
        job.paymentSystem = 10;
        job.forceInternalPos = true;
    }
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
        pt->setPaymentSystem(job.paymentSystem);
        if (job.forceInternalPos) {
            pt->setUseExtPosOverride(QStringLiteral("false"));
        }

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
            pt->makeJsonAndPrintSimple(job.taxDept, job.amountNonCash, job.amountPrepaid, "false");
        } else {
            pt->makeJsonAndPrint(job.amountCash, job.amountNonCash, job.amountPrepaid);
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
                          {"result", result},
                          {"f_fiscal_machine_id", mWorkStation.fiscalMachineId()}};

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
    if (mSkipOpenTableOnShow) {
        mSkipOpenTableOnShow = false;
        return;
    }
    NInterface::query1("/engine/v2/waiter/order/open-table",
                       fUser->mSessionKey,
                       this,
                       {{"table", tableId()}, {"locksrc", hostinfo}},
                       [=](const QJsonObject &jdoc) { parseOrder(jdoc); });
}

void WOrder::hideLoyaltyPanels()
{
    // По умолчанию виден только итоговый виджет; панели карт появляются по мере использования.
    ui->wDiscount->setVisible(false);
    ui->wGiftCard->setVisible(false);
    ui->wAccumulateCard->setVisible(false);

    // Старые смешанные поля внутри wTotal больше не используются.
    ui->lbDisc->setVisible(false);
    ui->leDisc->setVisible(false);
    ui->lbUseAccumulated->setVisible(false);
    ui->leUseAccumulated->setVisible(false);
    ui->leCurrentAccumulated->setVisible(false);
    ui->lbCurrentAccumlated->setVisible(false);
    ui->lbCardValidUntil->setVisible(false);
    ui->leCardValidUntil->setVisible(false);
    ui->leTotalAccumulated->setVisible(false);
}

void WOrder::updateLoyaltyLimits()
{
    const double total = mOrder.totalDue;
    if (mLoyalty.hasGift()) {
        double maxUse = qMin(mLoyalty.giftBalance, total);
        if (mLoyalty.useAmount > maxUse) {
            mLoyalty.useAmount = maxUse;
            ui->leUseAccumulated_3->setDouble(maxUse);
        }
    }
    if (mLoyalty.hasAccumulate()) {
        double maxUse = qMin(mLoyalty.accumulateBalance, total);
        if (mLoyalty.useAmount > maxUse) {
            mLoyalty.useAmount = maxUse;
            ui->leUseAccumulated_4->setDouble(maxUse);
        }
    }
}

void WOrder::clampUseAmount(C5LineEdit *le)
{
    const double entered = le->getDouble();
    double v = entered;
    double maxUse = qMin(mLoyalty.activeBalance(), mOrder.totalDue);
    if (v < 0) {
        v = 0;
    }
    if (v > maxUse) {
        v = maxUse;
    }
    if (qAbs(v - entered) > 1e-6) {
        le->setDouble(v);
    }
    mLoyalty.useAmount = v;
}

QJsonObject WOrder::loyaltyPayload() const
{
    QJsonObject jo;
    if (mLoyalty.hasDiscount()) {
        jo.insert("discount", QJsonObject{
            {"card_id", mLoyalty.discountCardId},
            {"partner_id", mLoyalty.discountPartnerId},
            {"type", mLoyalty.discountType},
            {"factor", mLoyalty.discountFactor},
        });
    }
    if (mLoyalty.hasGift() && mLoyalty.useAmount > 0.001) {
        jo.insert("gift", QJsonObject{
            {"card_id", mLoyalty.giftCardId},
            {"spend", mLoyalty.useAmount},
        });
    }
    if (mLoyalty.hasAccumulate()) {
        QJsonObject acc{
            {"card_id", mLoyalty.accumulateCardId},
            {"percent", mLoyalty.accumulatePercent},
        };
        if (mLoyalty.useAmount > 0.001) {
            acc.insert("spend", mLoyalty.useAmount);
        }
        jo.insert("accumulate", acc);
    }
    return jo;
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
    ui->tblData->setColumnDecimals(col_stock, 3);
    mOrder = JsonParser<WaiterOrder>::fromJson(jdoc["order"].toObject());

    const QJsonObject guest = mOrder.data.value(QStringLiteral("f_guest")).toObject();
    const int guestId = guest.value(QStringLiteral("f_guest_id")).toInt();
    if (guestId > 0) {
        mPartnerId = guestId;
        // После make-draft / loadExistingOrder UI пустой — поднимаем имя/ИИН из f_guest.
        // Не трогаем поля, если пользователь уже что-то ввёл локально.
        const QString tin = guest.value(QStringLiteral("f_guest_tin")).toString();
        const QString name = guest.value(QStringLiteral("f_guest_name")).toString();
        if (ui->leTIN->text().isEmpty() && !tin.isEmpty()) {
            ui->leTIN->setText(tin);
        }
        if (ui->leCustomer->text().isEmpty() && !name.isEmpty()) {
            ui->leCustomer->setText(name);
        }
    } else if (mPartnerId <= 0) {
        mPartnerId = 0;
    }
    // Если guestId == 0, но mPartnerId > 0 — партнёр выбран локально (часто до первого товара).
    // Не обнуляем: persistGuestToOrder вызовут после появления id заказа / перед оплатой.
    ui->btnF5->setChecked(mWorkStation.quickDebtPartnerId() == mPartnerId);
    ui->btnF5->setVisible(mPartnerId > 0);

    ui->tblData->setRowCount(mOrder.normalDishesCount());
    int i = 0;
    for (auto const &g : mOrder.dishes) {
        if (g.state != 1) {
            continue;
        }
        ui->tblData->setData(i, 0, QVariant::fromValue(g));
        ui->tblData->setString(i, col_barcode, g.barcode());
        ui->tblData->setString(i, col_group, g.dataValue(QStringLiteral("f_groupname")).toString());
        ui->tblData->setString(i, col_name, g.dishName);
        ui->tblData->setDouble(i, col_qty, g.qty);
        ui->tblData->setDouble(i, col_qtybox, g.dataValue(QStringLiteral("f_qtybox")).toDouble());
        ui->tblData->setDouble(i, col_price, g.price);
        ui->tblData->setString(i, col_unit, g.unitName);
        ui->tblData->setDouble(i, col_total, g.total(false));
        ui->tblData->setDouble(i, col_stock, g.dataValue(QStringLiteral("f_stock")).toDouble());
        ui->tblData->setRowHidden(i, g.state != 1);
        i++;
    }
    ui->leTotal->setDouble(mOrder.totalDue);
    if (ui->leComment->text() != mOrder.comment()) {
        ui->leComment->setText(mOrder.comment());
    }
    updateLoyaltyLimits();
    if (Working *w = fWorking ? fWorking : Working::working()) {
        if (w->worder() == this) {
            w->refreshCustomerDisplay();
        }
    }
}

double WOrder::qtyInOrderForDish(int dishId, const QString &excludeLineId) const
{
    double sum = 0;
    for (const auto &d : mOrder.dishes) {
        if (d.state != DISH_STATE_OK || d.dishId != dishId) {
            continue;
        }
        if (!excludeLineId.isEmpty() && d.id == excludeLineId) {
            continue;
        }
        sum += d.qty;
    }
    return sum;
}

bool WOrder::allowStockForDish(int dishId, bool isService, double stockQty, double addQty,
                               const QString &excludeLineId) const
{
    if (!mWorkStation.dontAllowNegativeRemains() || isService || dishId <= 0) {
        return true;
    }
    const double already = qtyInOrderForDish(dishId, excludeLineId);
    if (already + addQty > stockQty + 0.0001) {
        C5Message::error(tr("Insufficient stock") + QStringLiteral(": ")
                         + float_str(stockQty, 3) + QStringLiteral(" / ")
                         + float_str(already + addQty, 3));
        return false;
    }
    return true;
}

void WOrder::on_leComment_editingFinished()
{
    if (mOrder.id.isEmpty()) {
        return;
    }
    const QString comment = ui->leComment->text().trimmed();
    if (comment == mOrder.comment()) {
        return;
    }
    NInterface::query1(QStringLiteral("/engine/v2/waiter/order/set-header-comment"),
                       fUser->mSessionKey,
                       this,
                       {{QStringLiteral("id"), mOrder.id},
                        {QStringLiteral("comment"), comment}},
                       [this](const QJsonObject &jdoc) { parseOrder(jdoc); });
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
