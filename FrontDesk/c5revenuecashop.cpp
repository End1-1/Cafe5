#include "c5revenuecashop.h"
#include "ui_c5revenuecashop.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QtGlobal>
#include "format_date.h"
#include "c5cashoperationtypes.h"
#include "c5codenameselectorfunctions.h"
#include "c5message.h"
#include "dict_payment_type.h"
#include "format_date.h"
#include "ninterface.h"

namespace {

bool isCashOperationIncome(int opType)
{
    if (opType == 101) {
        return true;
    }
    switch (opType) {
    case CASH_OP_SALES_REVENUE:
    case CASH_OP_DEBT_RECOVERY:
    case CASH_OP_CASH_OVERAGE:
        return true;
    default:
        return false;
    }
}

} // namespace

C5RevenueCashOp::C5RevenueCashOp(C5User *user, QWidget *parent)
    : C5Dialog(user, parent)
    , ui(new Ui::C5RevenueCashOp)
{
    ui->setupUi(this);
    ui->cbOperation->clear();
    ui->cbOperation->addItem(tr("Income"), 1);
    ui->cbOperation->addItem(tr("Outcome"), 2);
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
    refillCashOperationTypeCombo();
    connect(ui->cbExpenseType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
        syncOperationFromExpenseType();
    });
}

void C5RevenueCashOp::refillCashOperationTypeCombo()
{
    const int prevType = ui->cbExpenseType->count() > 0 ? ui->cbExpenseType->currentData().toInt() : 0;

    ui->cbExpenseType->clear();
    ui->cbExpenseType->addItem(tr("Sales Revenue"), CASH_OP_SALES_REVENUE);
    ui->cbExpenseType->addItem(tr("Total Expenses"), CASH_OP_TOTAL_EXPENSES);
    ui->cbExpenseType->addItem(tr("Purchasing Costs"), CASH_OP_PURCHASING_COSTS);
    ui->cbExpenseType->addItem(tr("Salaries and Wages"), CASH_OP_SALARIES);
    ui->cbExpenseType->addItem(tr("Debt Recovery"), CASH_OP_DEBT_RECOVERY);
    ui->cbExpenseType->addItem(tr("Debt Repayment"), CASH_OP_DEBT_REPAYMENT);
    ui->cbExpenseType->addItem(tr("Utilities"), CASH_OP_UTILITIES);
    ui->cbExpenseType->addItem(tr("Cash Shortage"), CASH_OP_CASH_SHORTAGE);
    ui->cbExpenseType->addItem(tr("Cash Overage"), CASH_OP_CASH_OVERAGE);

    const int keepIdx = prevType > 0 ? ui->cbExpenseType->findData(prevType) : -1;
    if (keepIdx >= 0) {
        ui->cbExpenseType->setCurrentIndex(keepIdx);
    } else {
        const int defType = ui->cbOperation->currentIndex() == 0 ? CASH_OP_DEBT_RECOVERY : CASH_OP_TOTAL_EXPENSES;
        const int defIdx = ui->cbExpenseType->findData(defType);
        ui->cbExpenseType->setCurrentIndex(defIdx >= 0 ? defIdx : 0);
    }
    ui->cbExpenseType->setEnabled(true);
}

void C5RevenueCashOp::syncOperationFromExpenseType()
{
    const int opType = selectedOperationType();
    const int wantData = isCashOperationIncome(opType) ? 1 : 2;
    for (int i = 0; i < ui->cbOperation->count(); ++i) {
        if (ui->cbOperation->itemData(i).toInt() == wantData) {
            ui->cbOperation->setCurrentIndex(i);
            break;
        }
    }
}

int C5RevenueCashOp::selectedOperationType() const
{
    const int idx = ui->cbExpenseType->currentIndex();
    if (idx >= 0) {
        return ui->cbExpenseType->currentData().toInt();
    }
    return ui->cbOperation->currentIndex() == 0 ? CASH_OP_DEBT_RECOVERY : CASH_OP_TOTAL_EXPENSES;
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
    const bool income = isCashOperationIncome(opType);
    const int wantData = income ? 1 : 2;
    for(int i = 0; i < ui->cbOperation->count(); i++) {
        if(ui->cbOperation->itemData(i).toInt() == wantData) {
            ui->cbOperation->setCurrentIndex(i);
            break;
        }
    }
    refillCashOperationTypeCombo();

    const int typeIdx = ui->cbExpenseType->findData(opType);
    if (typeIdx >= 0) {
        ui->cbExpenseType->setCurrentIndex(typeIdx);
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

    const int operationType = selectedOperationType();
    const bool income = isCashOperationIncome(operationType);
    const int paymentTypeId = ui->cbPaymentType->currentData().toInt();
    const double debit = income ? amount : 0.0;
    const double credit = income ? 0.0 : amount;

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

