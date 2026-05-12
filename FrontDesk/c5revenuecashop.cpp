#include "c5revenuecashop.h"
#include "ui_c5revenuecashop.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QtGlobal>
#include "format_date.h"
#include "c5codenameselectorfunctions.h"
#include "c5message.h"
#include "dict_payment_type.h"
#include "format_date.h"
#include "ninterface.h"

C5RevenueCashOp::C5RevenueCashOp(C5User *user, QWidget *parent)
    : C5Dialog(user, parent)
    , ui(new Ui::C5RevenueCashOp)
{
    ui->setupUi(this);
    ui->cbOperation->setCurrentIndex(0);
    for(int pt : payment_types) {
        ui->cbPaymentType->addItem(QCoreApplication::translate("PaymentType", payment_names[pt]), pt);
    }
    const int cashIdx = ui->cbPaymentType->findData(PAYMENT_TYPE_CASH);
    if(cashIdx >= 0) {
        ui->cbPaymentType->setCurrentIndex(cashIdx);
    }
    ui->dsAmount->setDecimals(2);
    ui->dsAmount->setMinimum(0.00);
    ui->dsAmount->setMaximum(999999999.00);
    ui->deDateTime->setDateTime(QDateTime::currentDateTime());
}

C5RevenueCashOp::~C5RevenueCashOp()
{
    delete ui;
}

void C5RevenueCashOp::setCashboxAndCurrency(int cashboxId, int currencyId)
{
    mCashboxId = cashboxId;
    mCurrencyId = currencyId > 0 ? currencyId : 1;
    mOperationId = 0;
    ui->leCashbox->setText(mCashboxId > 0 ? QString::number(mCashboxId) : QString());
    ui->btnCashbox->setEnabled(true);
}

void C5RevenueCashOp::applyOperationForEdit(const QJsonObject &operation)
{
    mOperationId = operation.value(QStringLiteral("f_id")).toVariant().toInt();
    mCashboxId = operation.value(QStringLiteral("f_cashbox_id")).toVariant().toInt();
    mCurrencyId = operation.value(QStringLiteral("f_currency_id")).toVariant().toInt();
    if(mCurrencyId <= 0) {
        mCurrencyId = 1;
    }
    ui->leCashbox->setText(mCashboxId > 0 ? QString::number(mCashboxId) : QString());
    ui->btnCashbox->setEnabled(false);

    const int opType = operation.value(QStringLiteral("f_operation_type")).toInt();
    const bool income = opType == 101;
    const int wantData = income ? 1 : 2;
    for(int i = 0; i < ui->cbOperation->count(); i++) {
        if(ui->cbOperation->itemData(i).toInt() == wantData) {
            ui->cbOperation->setCurrentIndex(i);
            break;
        }
    }

    const int paymentTypeId = operation.value(QStringLiteral("f_payment_type_id")).toInt();
    const int ptIdx = ui->cbPaymentType->findData(paymentTypeId);
    if(ptIdx >= 0) {
        ui->cbPaymentType->setCurrentIndex(ptIdx);
    }

    const double debit = operation.value(QStringLiteral("f_debit")).toVariant().toDouble();
    const double credit = operation.value(QStringLiteral("f_credit")).toVariant().toDouble();
    const double amount = qMax(debit, credit);
    ui->dsAmount->setValue(amount);

    const QString dtStr = operation.value(QStringLiteral("f_datetime")).toString();
    QDateTime dt = QDateTime::fromString(dtStr, FORMAT_DATETIME_TO_STR_MYSQL);
    if(!dt.isValid()) {
        dt = QDateTime::currentDateTime();
    }
    ui->deDateTime->setDateTime(dt);

    ui->leComment->setText(operation.value(QStringLiteral("f_comment")).toString());
}

void C5RevenueCashOp::on_btnSave_clicked()
{
    if(mCashboxId <= 0) {
        C5Message::error(tr("Cashbox must be selected in filter"));
        return;
    }

    const double amount = ui->dsAmount->value();
    if(amount <= 0.009) {
        C5Message::error(tr("Amount must be greater than zero"));
        return;
    }

    const QString comment = ui->leComment->text().trimmed();
    if(comment.isEmpty()) {
        C5Message::error(tr("Comment is required"));
        return;
    }

    const bool income = ui->cbOperation->currentData().toInt() == 1;
    const int paymentTypeId = ui->cbPaymentType->currentData().toInt();
    const double debit = income ? amount : 0.0;
    const double credit = income ? 0.0 : amount;
    const int operationType = income ? 101 : 102;

    if(mOperationId > 0) {
        NInterface::query1("/engine/v2/waiter/cashbox/update-cash-operation",
                           mUser->mSessionKey,
                           this,
                           {
                               {"id", mOperationId},
                               {"f_operation_type", operationType},
                               {"f_payment_type_id", paymentTypeId},
                               {"f_debit", debit},
                               {"f_credit", credit},
                               {"f_datetime", ui->deDateTime->dateTime().toString(FORMAT_DATETIME_TO_STR_MYSQL)},
                               {"f_currency_id", mCurrencyId},
                               {"f_comment", comment}
                           },
                           [this](const QJsonObject &) {
                               accept();
                           });
        return;
    }

    NInterface::query1("/engine/v2/waiter/cashbox/move-money",
                       mUser->mSessionKey,
                       this,
                       {
                           {"cashbox_id", mCashboxId},
                           {"f_order_id", ""},
                           {"f_operation_type", operationType},
                           {"f_payment_type_id", paymentTypeId},
                           {"f_debit", debit},
                           {"f_credit", credit},
                           {"f_datetime", ui->deDateTime->dateTime().toString(FORMAT_DATETIME_TO_STR_MYSQL)},
                           {"f_currency_id", mCurrencyId},
                           {"f_comment", comment}
                       },
                       [this](const QJsonObject &) {
                           accept();
                       });
}

void C5RevenueCashOp::on_btnCancel_clicked()
{
    reject();
}

void C5RevenueCashOp::on_btnCashbox_clicked()
{
    const auto r = selectItem<StructCashbox>(true, false, ui->btnCashbox->mapToGlobal(QPoint(0, ui->btnCashbox->height())));
    if(r.isEmpty()) {
        return;
    }
    const auto &cb = r.first();
    mCashboxId = cb.id;
    ui->leCashbox->setText(cb.name);
}

