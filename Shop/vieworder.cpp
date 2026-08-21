#include "vieworder.h"
#include <QClipboard>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPointer>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrinterInfo>
#include <QSet>
#include <QThread>
#include <QUuid>
#include "c5checkbox.h"
#include "c5lineedit.h"
#include "c5message.h"
#include "c5printing.h"
#include "c5printrecipta4.h"
#include "c5user.h"
#include "c5utils.h"
#include "dlgdate.h"
#include "format_date.h"
#include "goodsreturnreason.h"
#include "jsons.h"
#include "ninterface.h"
#include "printreceiptgroup.h"
#include "printtaxn.h"
#include "selectprinters.h"
#include "struct_workstationitem.h"
#include "ui_vieworder.h"
#include "worder.h"
#include "working.h"

ViewOrder::ViewOrder(Working *w, const QString &order, C5User *user)
    : C5ShopDialog(user)
    , ui(new Ui::ViewOrder)
    , fWorking(w)
{
    ui->setupUi(this);
    showMaximized();
    fUuid = order;
    // UUID, X, Goods, Sold qty, Price, Total, GoodsId, Scancode, Service, Return qty, Store
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 40, 300, 90, 90, 100, 0, 140, 0, 110, 0);
    ui->tbl->setColumnDecimals(3, 3);
    ui->tbl->setColumnDecimals(9, 3);
    ui->btnSaveReturn->setVisible(false);
    if (auto *h = ui->tbl->horizontalHeaderItem(3)) {
        h->setText(tr("Sold"));
    }
    if (auto *h = ui->tbl->horizontalHeaderItem(9)) {
        h->setText(tr("Return qty"));
    }
    loadOrder();
}

ViewOrder::~ViewOrder()
{
    delete ui;
}

void ViewOrder::loadOrder()
{
    NInterface::query1(QStringLiteral("/engine/v2/shop/view-order/get"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("id"), fUuid}},
                       [this](const QJsonObject &jo) { fillFromJson(jo); });
}

void ViewOrder::fillFromJson(const QJsonObject &jo)
{
    fHeader = jo.value(QStringLiteral("header")).toObject();
    if (fHeader.isEmpty()) {
        C5Message::error(tr("Document is not exists"));
        return;
    }

    const QString number = fHeader.value(QStringLiteral("f_number")).toString();
    ui->leOrderNum->setText(number);
    ui->leAmount->setDouble(fHeader.value(QStringLiteral("f_amounttotal")).toDouble());
    ui->leDate->setDate(QDate::fromString(fHeader.value(QStringLiteral("f_datecash")).toString(), FORMAT_DATE_TO_STR_MYSQL));
    ui->leTime->setText(fHeader.value(QStringLiteral("f_time_close")).toString());
    ui->leUUID->setText(fHeader.value(QStringLiteral("f_id")).toString());
    ui->leCash->setDouble(fHeader.value(QStringLiteral("f_amount_cash")).toDouble());
    ui->leCard->setDouble(fHeader.value(QStringLiteral("f_amount_card")).toDouble());
    ui->leIdram->setDouble(fHeader.value(QStringLiteral("f_amount_idram")).toDouble());
    ui->leDebt->setDouble(fHeader.value(QStringLiteral("f_amount_debt")).toDouble());
    ui->leBank->setDouble(fHeader.value(QStringLiteral("f_amount_bank")).toDouble());
    ui->leTelcell->setDouble(fHeader.value(QStringLiteral("f_amount_telcell")).toDouble());
    ui->leSaler->setText(fHeader.value(QStringLiteral("f_saler")).toString());
    ui->leBuyer->setText(fHeader.value(QStringLiteral("f_buyer")).toString());
    ui->leDeliveryMan->setText(fHeader.value(QStringLiteral("f_deliveryman")).toString());

    fSaleDoc = QStringLiteral("%1, %2")
                   .arg(number, ui->leDate->date().toString(FORMAT_DATE_TO_STR));

    const QJsonObject fiscal = fHeader.value(QStringLiteral("f_fiscal")).toObject();
    const QString rseq = fiscal.contains(QStringLiteral("rseq"))
                             ? QString::number(fiscal.value(QStringLiteral("rseq")).toInt())
                             : QString();
    ui->leTaxNumber->setText(rseq);
    ui->btnPrintFiscal->setVisible(rseq.toInt() == 0);
    ui->btnTaxReturn->setVisible(!ui->btnPrintFiscal->isVisible());

    ui->tbl->setRowCount(0);
    const QJsonArray goods = jo.value(QStringLiteral("goods")).toArray();
    for (const QJsonValue &jv : goods) {
        const QJsonObject g = jv.toObject();
        const int r = ui->tbl->addEmptyRow();
        ui->tbl->setString(r, 0, g.value(QStringLiteral("f_id")).toString());
        auto *cb = ui->tbl->createCheckbox(r, 1);
        ui->tbl->setString(r, 2, g.value(QStringLiteral("f_name")).toString());
        ui->tbl->setDouble(r, 3, g.value(QStringLiteral("f_qty")).toDouble());
        ui->tbl->setDouble(r, 4, g.value(QStringLiteral("f_price")).toDouble());
        ui->tbl->setDouble(r, 5, g.value(QStringLiteral("f_total")).toDouble());
        ui->tbl->setInteger(r, 6, g.value(QStringLiteral("f_goodsid")).toInt());
        ui->tbl->setString(r, 7, g.value(QStringLiteral("f_scancode")).toString());
        ui->tbl->setString(r, 8, g.value(QStringLiteral("f_is_service")).toBool() ? QStringLiteral("1") : QStringLiteral("0"));
        // Col 9 = return qty (empty until return mode). Already returned kept in UserRole+3.
        ui->tbl->setDouble(r, 9, 0);
        ui->tbl->setInteger(r, 10, g.value(QStringLiteral("f_store")).toInt());
        ui->tbl->item(r, 0)->setData(Qt::UserRole + 1, g.value(QStringLiteral("f_fiscal_row")).toInt());
        ui->tbl->item(r, 0)->setData(Qt::UserRole + 2, g.value(QStringLiteral("f_qty_available")).toDouble());
        ui->tbl->item(r, 0)->setData(Qt::UserRole + 3, g.value(QStringLiteral("f_returnedqty")).toDouble());

        const double available = g.value(QStringLiteral("f_qty_available")).toDouble();
        if (available < 0.0001 || g.value(QStringLiteral("f_price")).toDouble() < 0) {
            cb->setEnabled(false);
        }
        connect(cb, &C5CheckBox::clicked, this, [this](bool checked) {
            if (!property("return").toBool()) {
                return;
            }
            int r = -1, c = -1;
            auto *senderCb = qobject_cast<C5CheckBox *>(sender());
            if (!senderCb || !ui->tbl->findWidget(senderCb, r, c) || r < 0) {
                return;
            }
            if (auto *le = ui->tbl->lineEdit(r, 9)) {
                if (checked && le->getDouble() < 0.0001) {
                    le->setDouble(ui->tbl->item(r, 0)->data(Qt::UserRole + 2).toDouble());
                    le->setFocus();
                    le->selectAll();
                } else if (!checked) {
                    le->setDouble(0);
                }
            }
            countOrder();
        });
    }

    if (ui->leAmount->getDouble() < 0) {
        ui->btnReturn->setVisible(false);
        ui->btnMakeDraft->setVisible(false);
    }
    ui->btnSaveReturn->setVisible(false);
    setProperty("return", false);
}

void ViewOrder::on_btnReturn_clicked()
{
    if (ui->leDate->date().daysTo(QDate::currentDate()) > 14) {
        C5Message::error(tr("You cannot return this item"));
        return;
    }

    NInterface::query1(QStringLiteral("/engine/v2/shop/view-order/return-reasons"),
                       mUser->mSessionKey,
                       this,
                       {},
                       [this](const QJsonObject &jo) {
                           GoodsReturnReason r(mUser);
                           r.setReasons(jo.value(QStringLiteral("reasons")).toArray());
                           if (r.exec() != QDialog::Accepted) {
                               return;
                           }
                           const int reason = r.fReason;
                           ui->leReturnReason->setProperty("reason", reason);
                           ui->leReturnReason->setText(r.fReasonName);
                           if (reason == 0) {
                               return;
                           }

                           setProperty("return", true);
                           setStyleSheet(QStringLiteral("background:rgb(255, 210, 217);"));
                           style()->polish(this);
                           ui->btnSaveReturn->setVisible(true);
                           ui->leUUID->setProperty("returnfrom", ui->leUUID->text());
                           ui->btnReturn->setEnabled(false);

                           if (auto *h = ui->tbl->horizontalHeaderItem(1)) {
                               h->setText(tr("Sel"));
                           }
                           if (auto *h = ui->tbl->horizontalHeaderItem(3)) {
                               h->setText(tr("Sold"));
                           }
                           if (auto *h = ui->tbl->horizontalHeaderItem(9)) {
                               h->setText(tr("Return qty"));
                           }

                           for (int i = 0; i < ui->tbl->rowCount(); i++) {
                               auto *cb = ui->tbl->checkBox(i, 1);
                               const double available = ui->tbl->item(i, 0)->data(Qt::UserRole + 2).toDouble();
                               if (!cb->isEnabled() || available < 0.0001) {
                                   ui->tbl->setDouble(i, 9, 0);
                                   continue;
                               }

                               auto *le = ui->tbl->createLineEdit(i, 9);
                               le->setDouble(0);
                               le->setPlaceholderText(float_str(available, 3));
                               le->setToolTip(tr("Enter return quantity (max %1)").arg(float_str(available, 3)));
                               connect(le, &C5LineEdit::textChanged, this, [this, le](const QString &) {
                                   int r = -1, c = -1;
                                   if (!ui->tbl->findWidget(le, r, c) || r < 0) {
                                       return;
                                   }
                                   const double maxQty = ui->tbl->item(r, 0)->data(Qt::UserRole + 2).toDouble();
                                   double v = le->getDouble();
                                   if (v < 0) {
                                       v = 0;
                                   }
                                   if (v > maxQty) {
                                       v = maxQty;
                                       le->setDouble(v);
                                   }
                                   auto *cbRow = ui->tbl->checkBox(r, 1);
                                   cbRow->setChecked(v > 0.0001);
                                   countOrder();
                               });
                           }
                           countOrder();
                           C5Message::info(tr("Mark rows and enter quantity in column \"Return qty\""));
                       });
}

void ViewOrder::countOrder()
{
    double total = 0;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        if (!property("return").toBool()) {
            ui->tbl->setDouble(i, 5, ui->tbl->getDouble(i, 3) * ui->tbl->getDouble(i, 4));
            continue;
        }
        double qty = 0;
        if (auto *le = ui->tbl->lineEdit(i, 9)) {
            qty = le->getDouble();
        } else {
            qty = ui->tbl->getDouble(i, 9);
        }
        if (!ui->tbl->checkBox(i, 1)->isChecked() || qty < 0.0001) {
            continue;
        }
        total += qty * ui->tbl->getDouble(i, 4);
    }
    if (property("return").toBool()) {
        ui->leAmount->setDouble(total);
    }
}

void ViewOrder::on_btnTaxReturn_clicked()
{
    const QJsonObject fiscal = fHeader.value(QStringLiteral("f_fiscal")).toObject();
    if (fiscal.isEmpty() || fiscal.value(QStringLiteral("rseq")).toInt() <= 0) {
        C5Message::error(tr("No fiscal exists for this order"));
        return;
    }

    QJsonObject fiscalOut;
    QString err;
    // Full ticket taxback (no returnItemList) — for cancelled fiscal without goods return
    FiscalMachine fm = getFiscalMachine(mWorkStation.fiscalMachineId());
    PrintTaxN pt(fm.ip, fm.port, fm.machinePassword, fm.externalPosString(), fm.opPin, fm.opPassword, this);
    QString jsnin, jsnout;
    const int result = pt.printTaxback(fiscal.value(QStringLiteral("rseq")).toInt(),
                                       fiscal.value(QStringLiteral("crn")).toString(),
                                       jsnin,
                                       jsnout,
                                       err);

    QJsonObject reply{{QStringLiteral("f_id"), QUuid::createUuid().toString(QUuid::WithoutBraces)},
                      {QStringLiteral("f_order"), fUuid},
                      {QStringLiteral("in"), QJsonDocument::fromJson(jsnin.toUtf8()).object()},
                      {QStringLiteral("out"), QJsonDocument::fromJson(jsnout.toUtf8()).object()},
                      {QStringLiteral("error"), err},
                      {QStringLiteral("result"), result},
                      {QStringLiteral("clear_fiscal"), true},
                      {QStringLiteral("f_fiscal_machine_id"), mWorkStation.fiscalMachineId()}};
    NInterface::query1(QStringLiteral("/engine/v2/common/fiscal/log"), mUser->mSessionKey, this, reply, [](const QJsonObject &) {});

    if (result != pt_err_ok) {
        C5Message::error(err.isEmpty() ? tr("Fiscal error") : err);
        return;
    }
    ui->leTaxNumber->clear();
    ui->btnPrintFiscal->setVisible(true);
    ui->btnTaxReturn->setVisible(false);
    C5Message::info(tr("Taxback complete"));
}

void ViewOrder::on_btnClose_clicked()
{
    accept();
}

void ViewOrder::on_btnCopyUUID_clicked()
{
    qApp->clipboard()->setText(ui->leUUID->text());
}

void ViewOrder::on_btnEditDeliveryMan_clicked()
{
    C5Message::info(tr("Not implemented"));
}

void ViewOrder::on_btnEditSaler_clicked()
{
    C5Message::info(tr("Not implemented"));
}

void ViewOrder::on_btnEditBuyer_clicked()
{
    C5Message::info(tr("Not implemented"));
}

void ViewOrder::on_btnPrintReceipt_clicked()
{
    PrintReceiptGroup::print2(ui->leUUID->text(), mUser, this);
}

void ViewOrder::on_btnPrintFiscal_clicked()
{
    if (C5Message::question(tr("Confirm to print fiscal")) != QDialog::Accepted) {
        return;
    }
    printCheckWithTax(ui->leUUID->text(), [this](auto rseq) { ui->leTaxNumber->setText(rseq); });
}

void ViewOrder::printCheckWithTax(const QString &id, std::function<void(const QString &)> funcSuccess)
{
    NInterface::query1("/engine/v2/common/fiscal/get", mUser->mSessionKey, this, {{"id", id}}, [this, id, funcSuccess](const QJsonObject &jo) {
        if (jo.value("fiscal").toInt() > 0) {
            C5Message::error(tr("Cannot print tax twice"));
            return;
        }

        QJsonObject jpartner = jo.value("partner").toObject();
        QJsonArray jgoods = jo.value("goods").toArray();
        const double cash = fHeader.value(QStringLiteral("f_amount_cash")).toDouble();
        const double card = fHeader.value(QStringLiteral("f_amount_card")).toDouble();
        const double idram = fHeader.value(QStringLiteral("f_amount_idram")).toDouble();
        const double telcell = fHeader.value(QStringLiteral("f_amount_telcell")).toDouble();
        const double prepaid = fHeader.value(QStringLiteral("f_amount_prepaid")).toDouble();
        const double nonCash = card + idram + telcell;
        int paymentSystem = -1;
        bool forceInternalPos = false;
        if (card >= idram && card >= telcell && card > 0.001) {
            paymentSystem = 1;
        } else if (idram >= telcell && idram > 0.001) {
            paymentSystem = 13;
            forceInternalPos = true;
        } else if (telcell > 0.001) {
            paymentSystem = 10;
            forceInternalPos = true;
        }

        FiscalMachine fm = getFiscalMachine(mWorkStation.fiscalMachineId());
        QString partnerTIN = jpartner.value("f_taxcode").toString();

        PrintTaxN *pt = new PrintTaxN(
            fm.ip, fm.port, fm.machinePassword, fm.externalPosString(), fm.opPin, fm.opPassword, this);
        pt->setPaymentSystem(paymentSystem);
        if (forceInternalPos) {
            pt->setUseExtPosOverride(QStringLiteral("false"));
        }

        if (partnerTIN.length() == 8 && ui->btnPrintPartnerTIN->isChecked()) {
            pt->fPartnerTin = partnerTIN;
        }

        for (int i = 0; i < jgoods.size(); i++) {
            const QJsonObject &jg = jgoods.at(i).toObject();
            pt->addGoods(jg.value("f_taxdept").toInt(),
                         jg.value("f_adgcode").toString(),
                         jg.value("f_goods").toString(),
                         jg.value("f_name").toString(),
                         jg.value("f_price").toDouble(),
                         jg.value("f_qty").toDouble(),
                         jg.value("f_discountfactor").toDouble() * 100);
        }

        connect(pt,
                &PrintTaxN::finished,
                this,
                [this, id, funcSuccess, pt](const QString &jsonIn, const QString &jsonOut, const QString &err, int result) {
                    QJsonObject reply{{"f_id", QUuid::createUuid().toString(QUuid::WithoutBraces)},
                                      {"f_order", id},
                                      {"in", QJsonDocument::fromJson(jsonIn.toUtf8()).object()},
                                      {"out", QJsonDocument::fromJson(jsonOut.toUtf8()).object()},
                                      {"error", err},
                                      {"result", result},
                                      {"f_fiscal_machine_id", mWorkStation.fiscalMachineId()}};

                    NInterface::query(
                        "/engine/v2/common/fiscal/log",
                        mUser->mSessionKey,
                        this,
                        reply,
                        [](const QJsonObject &) {},
                        [](const QJsonObject &) { return true; });

                    if (result == pt_err_ok) {
                        QJsonObject joutObj = QJsonDocument::fromJson(jsonOut.toUtf8()).object();
                        funcSuccess(QString::number(joutObj["rseq"].toInt()));
                        C5Message::info(tr("Printed"));
                    } else {
                        C5Message::error(err.isEmpty() ? tr("Fiscal error") : err);
                    }

                    pt->deleteLater();
                });

        pt->makeJsonAndPrint(cash, nonCash, prepaid);
    });
}

void ViewOrder::on_btnPrintReceiptA4_clicked()
{
    C5PrintReciptA4 p(ui->leUUID->text(), mUser, this);
    QString err;
    if (!p.print(err)) {
        C5Message::error(err);
    }
}

void ViewOrder::on_btnMakeDraft_clicked()
{
    if ((int)fHeader.value(QStringLiteral("f_state")).toInt() != 2) {
        C5Message::error(tr("Order is not closed"));
        return;
    }

    const QJsonObject fiscal = fHeader.value(QStringLiteral("f_fiscal")).toObject();
    const bool hasFiscal = fiscal.value(QStringLiteral("rseq")).toInt() > 0;

    const QString confirmMsg = hasFiscal
                                   ? tr("Fiscal receipt exists. Print taxback and return this sale to editing?")
                                   : tr("Return this sale to editing?");
    if (C5Message::question(confirmMsg) != QDialog::Accepted) {
        return;
    }

    QPointer<ViewOrder> self(this);

    auto runMakeDraft = [self]() {
        if (!self || !self->fWorking) {
            if (self) {
                C5Message::error(tr("Working window is not available"));
            }
            return;
        }
        int tableHint = self->fHeader.value(QStringLiteral("f_table")).toInt();
        QSet<int> used;
        if (self->fWorking->fTab) {
            for (int i = 0; i < self->fWorking->fTab->count(); ++i) {
                if (auto *wo = qobject_cast<WOrder *>(self->fWorking->fTab->widget(i))) {
                    used.insert(wo->tableId());
                }
            }
        }
        if (tableHint <= 0 || used.contains(tableHint)) {
            tableHint = self->fWorking->allocateFreeTableId();
        }
        if (tableHint <= 0) {
            C5Message::error(tr("No free tables in hall %1. Create more tables in h_tables or close unused sale tabs.")
                                 .arg(mWorkStation.defaultHallId()));
            return;
        }

        NInterface::query1(QStringLiteral("/engine/v2/shop/view-order/make-draft"),
                           self->mUser->mSessionKey,
                           self,
                           {{QStringLiteral("id"), self->fUuid},
                            {QStringLiteral("table"), tableHint},
                            {QStringLiteral("hall"), mWorkStation.defaultHallId()}},
                           [self](const QJsonObject &jo) {
                               if (!self || !self->fWorking) {
                                   return;
                               }
                               if (!self->fWorking->openExistingSale(jo)) {
                                   return;
                               }
                               self->accept();
                           });
    };

    // After successful KKM taxback: persist clear on server, then continue make-draft.
    auto afterFiscalCleared = [self, runMakeDraft]() {
        if (!self) {
            return;
        }
        self->fHeader.remove(QStringLiteral("f_fiscal"));
        self->fHeader.insert(QStringLiteral("f_fiscal_taxback_at"),
                             QDateTime::currentDateTime().toString(Qt::ISODate));
        self->ui->leTaxNumber->clear();
        self->ui->btnPrintFiscal->setVisible(true);
        self->ui->btnTaxReturn->setVisible(false);
        runMakeDraft();
    };

    auto clearFiscalOnServer = [self, afterFiscalCleared]() {
        if (!self) {
            return;
        }
        NInterface::query1(QStringLiteral("/engine/v2/shop/view-order/clear-fiscal"),
                           self->mUser->mSessionKey,
                           self,
                           {{QStringLiteral("id"), self->fUuid}},
                           [afterFiscalCleared](const QJsonObject &) { afterFiscalCleared(); });
    };

    if (!hasFiscal) {
        runMakeDraft();
        return;
    }

    FiscalMachine fm = getFiscalMachine(mWorkStation.fiscalMachineId());
    if (fm.id == 0) {
        C5Message::error(tr("Fiscal machine is not configured"));
        return;
    }

    PrintTaxN pt(fm.ip, fm.port, fm.machinePassword, fm.externalPosString(), fm.opPin, fm.opPassword, this);
    QString jsnin, jsnout, err;
    const int result = pt.printTaxback(fiscal.value(QStringLiteral("rseq")).toInt(),
                                       fiscal.value(QStringLiteral("crn")).toString(),
                                       jsnin,
                                       jsnout,
                                       err);

    QJsonObject reply{{QStringLiteral("f_id"), QUuid::createUuid().toString(QUuid::WithoutBraces)},
                      {QStringLiteral("f_order"), fUuid},
                      {QStringLiteral("in"), QJsonDocument::fromJson(jsnin.toUtf8()).object()},
                      {QStringLiteral("out"), QJsonDocument::fromJson(jsnout.toUtf8()).object()},
                      {QStringLiteral("error"), err},
                      {QStringLiteral("result"), result},
                      {QStringLiteral("clear_fiscal"), true},
                      {QStringLiteral("f_fiscal_machine_id"), mWorkStation.fiscalMachineId()}};

    if (result != pt_err_ok) {
        // Taxback already done on device, but DB still has fiscal — offer continue.
        const QString failMsg = err.isEmpty() ? tr("Fiscal error") : err;
        if (C5Message::question(
                failMsg + QStringLiteral("\n\n")
                + tr("If fiscal was already cancelled, clear it and continue make draft?"))
            != QDialog::Accepted) {
            return;
        }
        clearFiscalOnServer();
        return;
    }

    // Must persist clear BEFORE make-draft; otherwise retry tries KKM again and sticks.
    NInterface::query1(QStringLiteral("/engine/v2/common/fiscal/log"),
                       mUser->mSessionKey,
                       this,
                       reply,
                       [afterFiscalCleared](const QJsonObject &) {
                           // clear_fiscal already applied in InsertLog; refresh UI and continue.
                           afterFiscalCleared();
                       });
}

bool ViewOrder::printPartialTaxback(double returnAmount, QJsonObject &fiscalOut, QString &err)
{
    const QJsonObject fiscal = fHeader.value(QStringLiteral("f_fiscal")).toObject();
    const int rseq = fiscal.value(QStringLiteral("rseq")).toInt();
    const QString crn = fiscal.value(QStringLiteral("crn")).toString();
    if (rseq <= 0 || crn.isEmpty()) {
        return true; // no fiscal — ok
    }

    FiscalMachine fm = getFiscalMachine(mWorkStation.fiscalMachineId());
    if (fm.id == 0) {
        err = tr("Fiscal machine is not configured");
        return false;
    }

    PrintTaxN pt(fm.ip, fm.port, fm.machinePassword, fm.externalPosString(), fm.opPin, fm.opPassword, this);

    const double cashPaid = fHeader.value(QStringLiteral("f_amount_cash")).toDouble();
    const double cardPaid = fHeader.value(QStringLiteral("f_amount_card")).toDouble()
                            + fHeader.value(QStringLiteral("f_amount_idram")).toDouble()
                            + fHeader.value(QStringLiteral("f_amount_telcell")).toDouble();
    const double prepaidPaid = fHeader.value(QStringLiteral("f_amount_prepaid")).toDouble();

    if (cashPaid > 0.01) {
        pt.fCashAmountForReturn = returnAmount;
    } else if (cardPaid > 0.01) {
        pt.fCardAmountForReturn = returnAmount;
    } else if (prepaidPaid > 0.01) {
        pt.fPrepaymentAmountForReturn = returnAmount;
    } else {
        // debt/bank only — fiscal return often skipped
        return true;
    }

    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        if (!ui->tbl->checkBox(i, 1)->isChecked()) {
            continue;
        }
        double qty = 0;
        if (auto *le = ui->tbl->lineEdit(i, 9)) {
            qty = le->getDouble();
        } else {
            qty = ui->tbl->getDouble(i, 9);
        }
        if (qty < 0.0001) {
            continue;
        }
        const int fiscalRow = ui->tbl->item(i, 0)->data(Qt::UserRole + 1).toInt();
        pt.addReturnItem(fiscalRow, qty);
    }

    QString jsnin, jsnout;
    const int result = pt.printTaxback(rseq, crn, jsnin, jsnout, err);
    fiscalOut = QJsonObject{{QStringLiteral("in"), QJsonDocument::fromJson(jsnin.toUtf8()).object()},
                            {QStringLiteral("out"), QJsonDocument::fromJson(jsnout.toUtf8()).object()},
                            {QStringLiteral("error"), err},
                            {QStringLiteral("result"), result},
                            {QStringLiteral("err"), err},
                            {QStringLiteral("f_fiscal_machine_id"), mWorkStation.fiscalMachineId()}};

    NInterface::query1(QStringLiteral("/engine/v2/common/fiscal/log"),
                       mUser->mSessionKey,
                       this,
                       QJsonObject{{QStringLiteral("f_id"), QUuid::createUuid().toString(QUuid::WithoutBraces)},
                                   {QStringLiteral("f_order"), fUuid},
                                   {QStringLiteral("in"), fiscalOut.value(QStringLiteral("in"))},
                                   {QStringLiteral("out"), fiscalOut.value(QStringLiteral("out"))},
                                   {QStringLiteral("error"), err},
                                   {QStringLiteral("result"), result},
                                   {QStringLiteral("f_fiscal_machine_id"), mWorkStation.fiscalMachineId()}},
                       [](const QJsonObject &) {});

    return result == pt_err_ok;
}

void ViewOrder::submitReturn(const QJsonObject &fiscalInfo)
{
    QJsonArray items;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        if (!ui->tbl->checkBox(i, 1)->isChecked()) {
            continue;
        }
        double qty = 0;
        if (auto *le = ui->tbl->lineEdit(i, 9)) {
            qty = le->getDouble();
        } else {
            qty = ui->tbl->getDouble(i, 9);
        }
        if (qty < 0.0001) {
            continue;
        }
        items.append(QJsonObject{{QStringLiteral("id"), ui->tbl->getString(i, 0)},
                                 {QStringLiteral("qty"), qty}});
    }

    const int cashSessionId = Working::working()->cashSessionId();
    QJsonObject params{{QStringLiteral("id"), fUuid},
                       {QStringLiteral("reason_id"), ui->leReturnReason->property("reason").toInt()},
                       {QStringLiteral("items"), items},
                       {QStringLiteral("cash_session_id"), cashSessionId},
                       {QStringLiteral("cashbox_id"), mWorkStation.cashboxId()},
                       {QStringLiteral("store_id"), mWorkStation.defaultStoreId()}};
    if (!fiscalInfo.isEmpty()) {
        params.insert(QStringLiteral("fiscal"), fiscalInfo);
    }

    QPointer<ViewOrder> self(this);
    NInterface::query1(QStringLiteral("/engine/v2/shop/view-order/create-return"),
                       mUser->mSessionKey,
                       this,
                       params,
                       [self](const QJsonObject &jo) {
                           if (!self) {
                               return;
                           }
                           Q_UNUSED(jo);
                           C5Message::info(tr("Return completed"));
                           self->accept();
                       });
}

void ViewOrder::on_btnSaveReturn_clicked()
{
    if (ui->leReturnReason->property("reason").toInt() == 0) {
        C5Message::error(tr("The return reason must be specified."));
        return;
    }

    double returnAmount = 0;
    bool any = false;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        if (!ui->tbl->checkBox(i, 1)->isChecked()) {
            continue;
        }
        double qty = 0;
        if (auto *le = ui->tbl->lineEdit(i, 9)) {
            qty = le->getDouble();
        } else {
            qty = ui->tbl->getDouble(i, 9);
        }
        if (qty < 0.0001) {
            continue;
        }
        const double maxQty = ui->tbl->item(i, 0)->data(Qt::UserRole + 2).toDouble();
        if (qty > maxQty + 0.0001) {
            C5Message::error(tr("Invalid qty"));
            return;
        }
        any = true;
        returnAmount += qty * ui->tbl->getDouble(i, 4);
    }
    if (!any || returnAmount < 0.01) {
        C5Message::error(tr("Nothing to return"));
        return;
    }

    if (Working::working()->cashSessionId() <= 0) {
        C5Message::error(tr("Cashbox session is not open"));
        return;
    }
    if (mWorkStation.defaultStoreId() <= 0) {
        C5Message::error(tr("Store is not defined"));
        return;
    }

    ui->btnSaveReturn->setEnabled(false);

    QJsonObject fiscalInfo;
    QString err;
    if (!printPartialTaxback(returnAmount, fiscalInfo, err)) {
        ui->btnSaveReturn->setEnabled(true);
        C5Message::error(err.isEmpty() ? tr("Fiscal error") : err);
        return;
    }

    submitReturn(fiscalInfo);
}

void ViewOrder::on_tbl_cellClicked(int row, int column)
{
    if (!property("return").toBool()) {
        return;
    }
    if (!ui->tbl->checkBox(row, 1)->isEnabled()) {
        return;
    }

    // Click on goods name toggles selection and focuses return qty
    if (column == 2 || column == 1) {
        auto *cb = ui->tbl->checkBox(row, 1);
        if (column == 2) {
            cb->setChecked(!cb->isChecked());
        }
        if (cb->isChecked()) {
            if (auto *le = ui->tbl->lineEdit(row, 9)) {
                if (le->getDouble() < 0.0001) {
                    const double available = ui->tbl->item(row, 0)->data(Qt::UserRole + 2).toDouble();
                    le->setDouble(available);
                }
                le->setFocus();
                le->selectAll();
            }
        } else if (auto *le = ui->tbl->lineEdit(row, 9)) {
            le->setDouble(0);
        }
        countOrder();
    }
}

void ViewOrder::on_btnEditReason_clicked()
{
    on_btnReturn_clicked();
}

void ViewOrder::on_btnPrintPrices_clicked()
{
    C5Message::info(tr("Not implemented"));
}
