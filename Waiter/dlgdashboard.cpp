#include "dlgdashboard.h"
#include "c5cashoperationtypes.h"
#include "ui_dlgdashboard.h"
#include "c5message.h"
#include "ninterface.h"
#include "c5user.h"
#include "struct_workstationitem.h"
#include "c5permissions.h"
#include "dlgreports.h"
#include "dlgface.h"
#include "dlgrealreports.h"
#include "dlgmovemoney.h"
#include "format_date.h"
#include "format_date.h"
#include "c5printing.h"
#include "dict_payment_type.h"
#include "c5utils.h"
#include <QDateTime>
#include <QShowEvent>
#include <QPrinter>
#include <QPrinterInfo>
#include <QFile>
#include <QJsonArray>

DlgDashboard::DlgDashboard(const QJsonObject &jcashbox, C5User *user)
    : C5WaiterDialog(user),
      ui(new Ui::DlgDashboard),
      mCashboxData(jcashbox)
{
    ui->setupUi(this);
    ui->lbStaff->setText(user->shortFullName());
    setup();
}

DlgDashboard::~DlgDashboard()
{
    delete ui;
}

int DlgDashboard::exec()
{
    C5WaiterDialog::exec();
    return mRoute;
}

void DlgDashboard::showEvent(QShowEvent *e)
{
    C5WaiterDialog::showEvent(e);

    if(e->spontaneous()) {
        return;
    }

    if(mCashboxData.isEmpty()) {
        NInterface::query1("/engine/v2/waiter/cashbox/check-status", mUser->mSessionKey, this,
        {{"cashbox_id", mWorkStation.cashboxId()}},
        [ this ](const QJsonObject & jdoc) {
            mCashboxData = jdoc["cashbox_session"].toObject();
            setup();
        });
    }
}

void DlgDashboard::on_btnOpenCashbox_clicked()
{
    if(C5Message::question(tr("Open new cashbox session")) != QDialog::Accepted) {
        return;
    }

    double amountOpen = 0;
    const bool cashCounted = mWorkStation.data.value(QStringLiteral("input_cashbox_amount_before_close")).toBool();

    if (cashCounted) {
        DlgMoveMoney d(mUser);
        d.setMode(2);

        if (d.exec() != QDialog::Accepted) {
            return;
        }
        amountOpen = d.amount();
    }

    QJsonObject openParams;
    openParams[QStringLiteral("cashbox_id")] = mWorkStation.cashboxId();
    openParams[QStringLiteral("amount_open")] = amountOpen;
    if (cashCounted) {
        openParams[QStringLiteral("cash_counted")] = true;
    }

    NInterface::query1(QStringLiteral("/engine/v2/waiter/cashbox/open"),
                       mUser->mSessionKey,
                       this,
                       openParams,
                       [this](const QJsonObject &jdoc) {
                           mCashboxData = jdoc[QStringLiteral("cashbox_session")].toObject();
                           setup();

                           const QJsonObject reconcile = jdoc.value(QStringLiteral("open_reconcile")).toObject();
                           const double diffRaw = reconcile.value(QStringLiteral("f_amount_difference_raw")).toVariant().toDouble();
                           if (qAbs(diffRaw) > 0.009) {
                               QJsonObject act = mCashboxData;
                               for (auto it = reconcile.constBegin(); it != reconcile.constEnd(); ++it) {
                                   act[it.key()] = it.value();
                               }
                               C5Message::info((diffRaw > 0 ? tr("Cash Overage") : tr("Cash Shortage")) + ": "
                                               + reconcile.value(QStringLiteral("f_amount_difference")).toString());
                               printDifferenceAct(act, true);
                           }
                       });
}

void DlgDashboard::setup()
{
    if(mCashboxData.value("f_id").toInt() == 0) {
        ui->wsessionifno->setVisible(false);
        ui->wsessionstatus->setVisible(true);
        ui->btnOpenCashbox->setEnabled(mUser->check(cp_t5_waiter_open_close_shift));
        ui->btnCloseCashbox->setEnabled(mUser->check(cp_t5_waiter_open_close_shift));
        ui->btnCashIn->setEnabled(false);
        ui->btnCashOut->setEnabled(false);
    } else {
        ui->lbShiftNumber->setText(QString::number(mCashboxData["f_id"].toInt()));
        QDateTime shiftOpenDatetime = QDateTime::fromString(mCashboxData["f_date_open"].toString(), FORMAT_DATETIME_TO_STR_MYSQL);
        ui->lbShiftOpen->setText(shiftOpenDatetime.toString(FORMAT_DATETIME_TO_STR));
        ui->lbShiftOpenUser->setText(mCashboxData["f_user_open_name"].toString());
        ui->lbShiftOrdersCount->setText(QString::number(mCashboxData["f_orders_count"].toInt()));
        QString revenue = mCashboxData.value(QStringLiteral("f_amount_revenue")).toString();
        if (revenue.isEmpty()) {
            revenue = mCashboxData.value(QStringLiteral("f_amount_expected")).toString();
        }
        ui->lbShiftOrdersAmount->setText(revenue);
        ui->wsessionifno->setVisible(true);
        ui->wsessionstatus->setVisible(false);
        ui->btnOpenCashbox->setEnabled(false);
        ui->btnCloseCashbox->setEnabled(mUser->check(cp_t5_waiter_open_close_shift));
        ui->btnCashIn->setEnabled(mUser->check(cp_t5_waiter_cash_operations));
        ui->btnCashOut->setEnabled(mUser->check(cp_t5_waiter_cash_operations));
        ui->btnGoHall->setEnabled(mUser->check(cp_t5_waiter_edit_order));
    }

    ui->btnCashCollection->setEnabled(mUser->check(cp_t5_waiter_cash_operations));
    ui->btnOrders->setEnabled(mUser->check(cp_t5_waiter_reports));
    ui->btnReports->setEnabled(mUser->check(cp_t5_waiter_reports));
    refreshAttendanceStatus();
}

void DlgDashboard::updateAttendanceButtons()
{
    ui->btnCheckin->setEnabled(!mUser->active);
    ui->btnCheckout->setEnabled(mUser->active);
}

void DlgDashboard::refreshAttendanceStatus()
{
    NInterface::query1(QStringLiteral("/engine/v2/common/attendance/open"),
                       mUser->mSessionKey,
                       this,
                       {},
                       [this](const QJsonObject &jo) {
                           mUser->active = jo.value(QStringLiteral("active")).toBool();
                           updateAttendanceButtons();
                       });
}

void DlgDashboard::on_btnCloseCashbox_clicked()
{
    if(C5Message::question(tr("Do you want to close active session?")) != QDialog::Accepted) {
        return ;
    }

    double amount_cash = 0;
    const bool cashCounted = mWorkStation.data.value(QStringLiteral("input_cashbox_amount_before_close")).toBool();

    if (cashCounted) {
        DlgMoveMoney d(mUser);
        d.setMode(2);

        if (d.exec() != QDialog::Accepted) {
            return;
        }
        amount_cash = d.amount();
    }

    QJsonObject closeParams;
    closeParams[QStringLiteral("cashbox_id")] = mWorkStation.cashboxId();
    closeParams[QStringLiteral("amount_cash")] = amount_cash;
    if (cashCounted) {
        closeParams[QStringLiteral("cash_counted")] = true;
    }

    NInterface::query1(QStringLiteral("/engine/v2/waiter/cashbox/close"),
                       mUser->mSessionKey,
                       this,
                       closeParams,
                       [this, cashCounted](const QJsonObject &jdoc) {
                           mCashboxData = jdoc.value("cashbox").toObject();
                           C5Printing p;
                           QPrinterInfo pi = QPrinterInfo::printerInfo(mWorkStation.waiterReportPrinter());
                           QPrinter printer(pi);
                           printer.setPageSize(QPageSize::Custom);
                           printer.setFullPage(false);
                           QRectF pr = printer.pageRect(QPrinter::DevicePixel);
                           constexpr qreal SAFE_RIGHT_MM = 4.0;
                           qreal safePx = SAFE_RIGHT_MM * printer.logicalDpiX() / 25.4;
                           p.setSceneParams(pr.width() - safePx, pr.height(), printer.logicalDpiX());
                           p.setFont(qApp->font());
                           int bs = 22;
                           p.setFontSize(bs);
                           QString logoFile = qApp->applicationDirPath() + "/logo_receipt.png";

                           if (QFile::exists(logoFile)) {
                               p.image(logoFile, Qt::AlignHCenter);
                               p.br();
                           }

                           p.ctext(tr("Closing session") + " " + QString::number(mCashboxData.value("f_id").toInt()));
                           p.br();
                           p.br();
                           p.lrtext(tr("Open"), mCashboxData.value("f_date_open").toString());
                           p.br();
                           p.rtext(mCashboxData["f_user_open_name"].toString());
                           p.br();
                           p.br();
                           p.lrtext(tr("Close"), mCashboxData.value("f_date_close").toString());
                           p.br();
                           p.rtext(mCashboxData["f_user_close_name"].toString());
                           p.br();
                           p.br();
                           p.lrtext(tr("Operations"), QString::number(mCashboxData.value("f_orders_count").toInt()));
                           p.br();
                           {
                               QString shiftTotal = mCashboxData.value(QStringLiteral("f_amount_revenue")).toString();
                               if (shiftTotal.isEmpty()) {
                                   shiftTotal = mCashboxData.value(QStringLiteral("f_amount_expected")).toString();
                               }
                               p.lrtext(tr("Shift total"), shiftTotal);
                           }
                           p.br();
                           {
                               const QJsonArray salesByPay = mCashboxData.value(QStringLiteral("sales_by_payment")).toArray();
                               if (!salesByPay.isEmpty()) {
                                   p.line();
                                   p.br();
                                   p.ltext(tr("By payment type"), 0);
                                   p.br();
                                   for (const QJsonValue &v : salesByPay) {
                                       const QJsonObject row = v.toObject();
                                       const double net = row.value(QStringLiteral("f_net")).toDouble();
                                       if (qAbs(net) < 0.009) {
                                           continue;
                                       }
                                       QString name = row.value(QStringLiteral("f_name")).toString();
                                       QString amount = row.value(QStringLiteral("f_net_fmt")).toString();
                                       if (amount.isEmpty()) {
                                           amount = float_str(net, 2);
                                       }
                                       p.lrtext(name, amount);
                                       p.br();
                                   }
                                   p.line();
                                   p.br();
                               }
                           }
                           if (cashCounted) {
                               const QString expectedCash = mCashboxData.value(QStringLiteral("f_amount_expected_cash")).toString();
                               if (!expectedCash.isEmpty()) {
                                   p.lrtext(tr("Expected cash"), expectedCash);
                                   p.br();
                               }
                               p.lrtext(tr("Counted cash"), mCashboxData[QStringLiteral("f_amount_fact")].toString());
                               p.br();
                           }
                           const double diffRaw = mCashboxData[QStringLiteral("f_amount_difference_raw")].toVariant().toDouble();
                           if (qAbs(diffRaw) > 0.009) {
                               const QString diffLabel = diffRaw > 0
                                   ? tr("Cash Overage")
                                   : tr("Cash Shortage");
                               p.lrtext(diffLabel, mCashboxData[QStringLiteral("f_amount_difference")].toString());
                               p.br();
                           }
                           p.line();
                           p.br();
                           p.print(printer);

                           if (cashCounted) {
                               const double diffRaw = mCashboxData[QStringLiteral("f_amount_difference_raw")].toVariant().toDouble();
                               if (qAbs(diffRaw) > 0.009) {
                                   const QString diffLabel = diffRaw > 0
                                       ? tr("Cash Overage")
                                       : tr("Cash Shortage");
                                   C5Message::info(diffLabel + ": " + mCashboxData[QStringLiteral("f_amount_difference")].toString());
                                   printDifferenceAct(mCashboxData);
                               }
                           }

                           mCashboxData["f_id"] = 0;
                           setup();
                       });
}

void DlgDashboard::printDifferenceAct(const QJsonObject &cashbox, bool opening)
{
    C5Printing p;
    QPrinterInfo pi = QPrinterInfo::printerInfo(mWorkStation.waiterReportPrinter());
    QPrinter printer(pi);
    printer.setPageSize(QPageSize::Custom);
    printer.setFullPage(false);
    QRectF pr = printer.pageRect(QPrinter::DevicePixel);
    constexpr qreal SAFE_RIGHT_MM = 4.0;
    qreal safePx = SAFE_RIGHT_MM * printer.logicalDpiX() / 25.4;
    p.setSceneParams(pr.width() - safePx, pr.height(), printer.logicalDpiX());
    p.setFont(qApp->font());
    p.setFontSize(22);

    const double diffRaw = cashbox.value(QStringLiteral("f_amount_difference_raw")).toVariant().toDouble();
    const bool overage = diffRaw > 0;

    p.ctext(overage ? tr("Cash Overage Act") : tr("Cash Shortage Act"));
    p.br();
    p.ctext(tr("Session") + " " + QString::number(cashbox.value(QStringLiteral("f_id")).toInt()));
    p.br();
    p.br();
    if (opening) {
        p.lrtext(tr("Open"), cashbox.value(QStringLiteral("f_date_open")).toString());
        p.br();
        p.rtext(cashbox.value(QStringLiteral("f_user_open_name")).toString());
    } else {
        p.lrtext(tr("Close"), cashbox.value(QStringLiteral("f_date_close")).toString());
        p.br();
        p.rtext(cashbox.value(QStringLiteral("f_user_close_name")).toString());
    }
    p.br();
    p.br();
    const QString expectedCash = cashbox.value(QStringLiteral("f_amount_expected_cash")).toString();
    p.lrtext(expectedCash.isEmpty() ? tr("Expected amount") : tr("Expected cash"),
             expectedCash.isEmpty() ? cashbox.value(QStringLiteral("f_amount_expected")).toString() : expectedCash);
    p.br();
    p.lrtext(tr("Counted cash"), cashbox.value(QStringLiteral("f_amount_fact")).toString());
    p.br();
    p.lrtext(overage ? tr("Cash Overage") : tr("Cash Shortage"),
             cashbox.value(QStringLiteral("f_amount_difference")).toString());
    p.br();
    p.line();
    p.br();
    p.br();
    p.lrtext(tr("Signature"), QStringLiteral("______________"));
    p.br();
    p.print(printer);
}

void DlgDashboard::on_btnGoHall_clicked()
{
    mRoute = 3;
    accept();
}

void DlgDashboard::on_btnOrders_clicked()
{
    DlgReports d(mUser);
    d.exec();
}

void DlgDashboard::on_btnClose_clicked()
{
    reject();
}

void DlgDashboard::on_btnReports_clicked()
{
    DlgRealReports d(mUser);
    d.exec();
}

void DlgDashboard::on_btnCheckin_clicked()
{
    if(C5Message::question(tr("Confirm to checkin")) != QDialog::Accepted) {
        return;
    }

    NInterface::query1("/engine/v2/common/attendance/checkin", mUser->mSessionKey, this, {}, [this](const QJsonObject &jo) {
        mUser->active = jo.value(QStringLiteral("active")).toBool();
        updateAttendanceButtons();
        C5Message::info(tr("Checkin completed"));
    });
}

void DlgDashboard::on_btnCheckout_clicked()
{
    if(C5Message::question(tr("Confirm to checkout")) != QDialog::Accepted) {
        return;
    }

    NInterface::query1("/engine/v2/common/attendance/checkout", mUser->mSessionKey, this, {}, [this](const QJsonObject &jo) {
        mUser->active = jo.value(QStringLiteral("active")).toBool();
        updateAttendanceButtons();
        C5Message::info(tr("Checkout completed"));
    });
}

void DlgDashboard::on_btnCashIn_clicked()
{
    if(mCashboxData.value("f_id").toInt() == 0) {
        C5Message::error(tr("No active cashbox session"));
        return;
    }

    DlgMoveMoney d(mUser);
    d.setMode(1);

    if(d.exec() != QDialog::Accepted) {
        return;
    }

    NInterface::query1("/engine/v2/waiter/cashbox/move-money",
                       mUser->mSessionKey,
                       this,
                       {{"cashbox_id", mWorkStation.cashboxId()},
                        {"f_order_id", ""},
                        {"f_operation_type", CASH_OP_DEBT_RECOVERY},
                        {"f_payment_type_id", PAYMENT_TYPE_CASH},
                        {"f_debit", d.amount()},
                        {"f_credit", 0},
                        {"f_currency_id", 1},
                        {"f_comment", d.comment()}},
                       [this](const QJsonObject &) {
                           NInterface::query1("/engine/v2/waiter/cashbox/check-status", mUser->mSessionKey, this,
                           {{"cashbox_id", mWorkStation.cashboxId()}},
                           [this](const QJsonObject &jdoc) {
                               mCashboxData = jdoc["cashbox_session"].toObject();
                               setup();
                           });
                       });
}

void DlgDashboard::on_btnCashOut_clicked()
{
    if(mCashboxData.value("f_id").toInt() == 0) {
        C5Message::error(tr("No active cashbox session"));
        return;
    }

    const int cashboxId = mWorkStation.data.value(QStringLiteral("f_cashbox_id")).toVariant().toInt();
    if(cashboxId <= 0) {
        C5Message::error(tr("Please select a cashbox"));
        return;
    }

    DlgMoveMoney d(mUser);
    d.setMode(0);
    d.setExpenseTypes(true);
    QString cashboxName = mCashboxData.value(QStringLiteral("f_cashbox_name")).toString().trimmed();
    if(cashboxName.isEmpty()) {
        cashboxName = QString::number(cashboxId);
    }
    d.setCashboxName(cashboxName);

    if(d.exec() != QDialog::Accepted) {
        return;
    }

    int currencyId = mCashboxData.value(QStringLiteral("f_currency_id")).toVariant().toInt();
    if(currencyId <= 0) {
        currencyId = 1;
    }

    NInterface::query1("/engine/v2/waiter/cashbox/move-money",
                       mUser->mSessionKey,
                       this,
                       {{"cashbox_id", cashboxId},
                        {"f_order_id", ""},
                        {"f_operation_type", d.operationType()},
                        {"f_payment_type_id", PAYMENT_TYPE_CASH},
                        {"f_debit", 0},
                        {"f_credit", d.amount()},
                        {"f_currency_id", currencyId},
                        {"f_comment", d.comment()}},
                       [this, cashboxId](const QJsonObject &) {
                           NInterface::query1("/engine/v2/waiter/cashbox/check-status", mUser->mSessionKey, this,
                           {{"cashbox_id", cashboxId}},
                           [this](const QJsonObject &jdoc) {
                               mCashboxData = jdoc["cashbox_session"].toObject();
                               setup();
                           });
                       });
}

void DlgDashboard::on_btnCashCollection_clicked()
{
    const int cashboxId = mWorkStation.cashboxId();
    if (cashboxId <= 0) {
        C5Message::error(tr("Please select a cashbox"));
        return;
    }

    DlgMoveMoney d(mUser);
    d.setMode(4);
    QString cashboxName = mCashboxData.value(QStringLiteral("f_cashbox_name")).toString().trimmed();
    if (cashboxName.isEmpty()) {
        cashboxName = QString::number(cashboxId);
    }
    d.setCashboxName(cashboxName);
    const double expectedCash = mCashboxData.value(QStringLiteral("f_amount_expected_cash_raw")).toVariant().toDouble();
    if (expectedCash > 0.009) {
        d.setAmount(expectedCash);
    }

    if (d.exec() != QDialog::Accepted) {
        return;
    }

    const double amount = d.amount();
    const QString comment = d.comment().trimmed();
    if (C5Message::question(tr("Confirm cash collection of %1").arg(float_str(amount, 2))) != QDialog::Accepted) {
        return;
    }

    NInterface::query1(QStringLiteral("/engine/v2/waiter/cashbox/cash-collection"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("cashbox_id"), cashboxId},
                        {QStringLiteral("amount"), amount},
                        {QStringLiteral("comment"), comment}},
                       [this, cashboxId, cashboxName, amount, comment](const QJsonObject &jo) {
                           const int destId = jo.value(QStringLiteral("dest_cashbox_id")).toVariant().toInt();
                           printCashCollection(cashboxName, amount, destId, comment);
                           C5Message::info(destId > 0
                                               ? tr("Cash collection completed, moved to cashbox %1").arg(destId)
                                               : tr("Cash collection completed"));

                           NInterface::query1(QStringLiteral("/engine/v2/waiter/cashbox/check-status"),
                                              mUser->mSessionKey,
                                              this,
                                              {{QStringLiteral("cashbox_id"), cashboxId}},
                                              [this](const QJsonObject &jdoc) {
                                                  mCashboxData = jdoc[QStringLiteral("cashbox_session")].toObject();
                                                  setup();
                                              });
                       });
}

void DlgDashboard::printCashCollection(const QString &cashboxName, double amount, int destCashboxId, const QString &comment)
{
    C5Printing p;
    QPrinterInfo pi = QPrinterInfo::printerInfo(mWorkStation.waiterReportPrinter());
    QPrinter printer(pi);
    printer.setPageSize(QPageSize::Custom);
    printer.setFullPage(false);
    QRectF pr = printer.pageRect(QPrinter::DevicePixel);
    constexpr qreal SAFE_RIGHT_MM = 4.0;
    qreal safePx = SAFE_RIGHT_MM * printer.logicalDpiX() / 25.4;
    p.setSceneParams(pr.width() - safePx, pr.height(), printer.logicalDpiX());
    p.setFont(qApp->font());
    p.setFontSize(22);

    p.ctext(tr("Cash collection"));
    p.br();
    p.br();
    p.lrtext(tr("Date"), QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.lrtext(tr("Cashbox"), cashboxName);
    p.br();
    if (destCashboxId > 0) {
        p.lrtext(tr("Moved to cashbox"), QString::number(destCashboxId));
        p.br();
    }
    p.lrtext(tr("Staff"), mUser->shortFullName());
    p.br();
    if (!comment.isEmpty()) {
        p.ltext(comment, 0);
        p.br();
    }
    p.line();
    p.br();
    p.lrtext(tr("Amount"), float_str(amount, 2));
    p.br();
    p.line();
    p.br();
    p.br();
    p.lrtext(tr("Handed over"), QStringLiteral("______________"));
    p.br();
    p.br();
    p.lrtext(tr("Received"), QStringLiteral("______________"));
    p.br();
    p.print(printer);
}
