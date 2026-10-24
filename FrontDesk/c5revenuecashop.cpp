#include "c5revenuecashop.h"
#include "ui_c5revenuecashop.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QtGlobal>
#include "format_date.h"
#include "c5cache.h"
#include "c5cashoperationtypes.h"
#include "c5message.h"
#include "dict_payment_type.h"
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
    fillPaymentCombo();
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

void C5RevenueCashOp::fillPaymentCombo()
{
    ui->cbPayment->clear();
    C5Cache *cache = C5Cache::cache(cache_cash_names);
    if(cache) {
        for(int r = 0; r < cache->rowCount(); ++r) {
            const int id = cache->getInt(r, 0);
            const QString name = cache->getString(r, 1).trimmed();
            if(id <= 0 || name.isEmpty()) {
                continue;
            }
            const int idx = ui->cbPayment->count();
            ui->cbPayment->addItem(name);
            ui->cbPayment->setItemData(idx, PAYMENT_TYPE_CASH, Qt::UserRole);
            ui->cbPayment->setItemData(idx, id, Qt::UserRole + 1);
        }
    }
    for(int pt : payment_types) {
        if(pt == PAYMENT_TYPE_CASH) {
            continue;
        }
        const char *nm = payment_names.value(pt);
        const QString title = nm
            ? QCoreApplication::translate("PaymentType", nm)
            : QString::number(pt);
        const int idx = ui->cbPayment->count();
        ui->cbPayment->addItem(title);
        ui->cbPayment->setItemData(idx, pt, Qt::UserRole);
        ui->cbPayment->setItemData(idx, 0, Qt::UserRole + 1);
    }
}

void C5RevenueCashOp::selectPayment(int paymentTypeId, int cashboxId)
{
    const int wantType = paymentTypeId > 0 ? paymentTypeId : PAYMENT_TYPE_CASH;
    for(int i = 0; i < ui->cbPayment->count(); ++i) {
        const int pt = ui->cbPayment->itemData(i, Qt::UserRole).toInt();
        const int cb = ui->cbPayment->itemData(i, Qt::UserRole + 1).toInt();
        if(wantType == PAYMENT_TYPE_CASH) {
            if(pt == PAYMENT_TYPE_CASH && cb == cashboxId) {
                ui->cbPayment->setCurrentIndex(i);
                return;
            }
        } else if(pt == wantType) {
            ui->cbPayment->setCurrentIndex(i);
            return;
        }
    }
}

int C5RevenueCashOp::selectedPaymentType() const
{
    const int idx = ui->cbPayment->currentIndex();
    if(idx < 0) {
        return 0;
    }
    return ui->cbPayment->itemData(idx, Qt::UserRole).toInt();
}

int C5RevenueCashOp::selectedCashboxId() const
{
    const int idx = ui->cbPayment->currentIndex();
    if(idx < 0) {
        return mCashboxId;
    }
    const int pt = ui->cbPayment->itemData(idx, Qt::UserRole).toInt();
    const int cb = ui->cbPayment->itemData(idx, Qt::UserRole + 1).toInt();
    if(pt == PAYMENT_TYPE_CASH && cb > 0) {
        return cb;
    }
    return mCashboxId;
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
    selectPayment(PAYMENT_TYPE_CASH, mCashboxId);
}

void C5RevenueCashOp::applyOperationForEdit(const QJsonObject &operation)
{
    mOperationId = operation.value(QStringLiteral("f_id")).toVariant().toInt();
    mCashboxId = operation.value(QStringLiteral("f_cashbox_id")).toVariant().toInt();
    mCurrencyId = operation.value(QStringLiteral("f_currency_id")).toVariant().toInt();
    if(mCurrencyId <= 0) {
        mCurrencyId = 1;
    }
    const int opType = operation.value(QStringLiteral("f_operation_type")).toVariant().toInt();
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

    const int paymentTypeId = operation.value(QStringLiteral("f_payment_type_id")).toVariant().toInt();
    selectPayment(paymentTypeId, mCashboxId);

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
    const int paymentTypeId = selectedPaymentType();
    const int cashboxId = selectedCashboxId();
    if(paymentTypeId <= 0 || cashboxId <= 0) {
        C5Message::error(tr("Cashbox must be selected in filter"));
        return;
    }
    mCashboxId = cashboxId;

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
    const double debit = income ? amount : 0.0;
    const double credit = income ? 0.0 : amount;

    if(mOperationId > 0) {
        NInterface::query1("/engine/v2/waiter/cashbox/update-cash-operation",
                           mUser->mSessionKey,
                           this,
                           {
                               {"id", mOperationId},
                               {"f_operation_type", operationType},
                               {"f_cashbox_id", mCashboxId},
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
                           {"cashbox_id", cashboxId},
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

