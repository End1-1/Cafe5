#include "dlgmovemoney.h"
#include "dlgqty.h"
#include "dlgtext.h"
#include "ui_dlgmovemoney.h"
#include "c5cashoperationtypes.h"
#include "c5message.h"
#include "dict_payment_type.h"
#include "paymenttypenames.h"

DlgMoveMoney::DlgMoveMoney(C5User *user)
    : C5Dialog(user),
      ui(new Ui::DlgMoveMoney)
{
    ui->setupUi(this);
}

DlgMoveMoney::~DlgMoveMoney()
{
    delete ui;
}

void DlgMoveMoney::setAmount(double value)
{
    ui->leAmount->setDouble(value);
}

void DlgMoveMoney::setExpenseTypes(bool enabled)
{
    mShowExpenseTypes = enabled;
    ui->lbExpenseType->setVisible(enabled);
    ui->cbExpenseType->setVisible(enabled);
    if(!enabled) {
        return;
    }

    ui->cbExpenseType->clear();
    ui->cbExpenseType->addItem(tr("Total Expenses"), CASH_OP_TOTAL_EXPENSES);
    ui->cbExpenseType->addItem(tr("Purchasing Costs"), CASH_OP_PURCHASING_COSTS);
    ui->cbExpenseType->addItem(tr("Salaries and Wages"), CASH_OP_SALARIES);
    ui->cbExpenseType->addItem(tr("Debt Repayment"), CASH_OP_DEBT_REPAYMENT);
    ui->cbExpenseType->addItem(tr("Utilities"), CASH_OP_UTILITIES);
    ui->cbExpenseType->addItem(tr("Cash Shortage"), CASH_OP_CASH_SHORTAGE);
    const int idx = ui->cbExpenseType->findData(CASH_OP_TOTAL_EXPENSES);
    ui->cbExpenseType->setCurrentIndex(idx >= 0 ? idx : 0);
}

void DlgMoveMoney::setCashboxName(const QString &name)
{
    ui->lbCashbox->setVisible(true);
    ui->leCashbox->setVisible(true);
    ui->leCashbox->setText(name);
}

void DlgMoveMoney::setMode(int mode)
{
    mMode = mode;
    switch (mode) {
    case 0:
        ui->lbTitle->setText(tr("Cash out"));
        break;
    case 1:
        ui->lbTitle->setText(tr("Cash in"));
        break;
    case 2:
        ui->lbTitle->setText(tr("Cash in drawer"));
        break;
    case 3:
        ui->lbTitle->setText(PaymentTypeNames::title(PAYMENT_TYPE_PREPAID));
        break;
    case 4:
        ui->lbTitle->setText(tr("Cash collection"));
        break;
    }
}

double DlgMoveMoney::amount() const
{
    return ui->leAmount->getDouble();
}

int DlgMoveMoney::operationType() const
{
    if(!mShowExpenseTypes) {
        return 0;
    }
    return ui->cbExpenseType->currentData().toInt();
}

QString DlgMoveMoney::comment() const
{
    return ui->leComment->text();
}

void DlgMoveMoney::on_btnAmountKbd_clicked()
{
    double v = ui->leAmount->getDouble();

    if(!DlgQty::getQty(v, tr("Amount"), mUser)) {
        return;
    }

    ui->leAmount->setDouble(v);
}

void DlgMoveMoney::on_btnCommentKbd_clicked()
{
    QString txt = ui->leComment->text();

    if(!DlgText::getText(mUser, tr("Comment"), txt)) {
        return;
    }

    ui->leComment->setText(txt);
}

void DlgMoveMoney::on_btnSave_clicked()
{
    const bool zeroAllowed = mMode == 2;
    if(amount() < (zeroAllowed ? 0.0 : 0.001)) {
        C5Message::error(tr("Amount is not valid"));
        return;
    }

    if(mShowExpenseTypes) {
        if(operationType() <= 0) {
            C5Message::error(tr("Operation type"));
            return;
        }
        if(comment().trimmed().isEmpty()) {
            C5Message::error(tr("Comment is required"));
            return;
        }
    }

    accept();
}

void DlgMoveMoney::on_btnCancel_clicked()
{
    reject();
}
