#include "dlgdebtsredeem.h"
#include "ui_dlgdebtsredeem.h"
#include "c5codenameselectorfunctions.h"
#include "c5message.h"
#include "dict_payment_type.h"
#include "format_date.h"
#include <QCoreApplication>
#include <QDate>
#include <QDialogButtonBox>

namespace {

const QVector<int> kDebtRedeemPaymentTypes = {
    PAYMENT_TYPE_CASH,
    PAYMENT_TYPE_CARD,
    PAYMENT_TYPE_BANK,
};

} // namespace

DlgDebtsRedeem::DlgDebtsRedeem(QWidget *parent, int docType, int currencyId)
    : QDialog(parent)
    , ui(new Ui::DlgDebtsRedeem)
{
    ui->setupUi(this);
    ui->deDate->setDate(QDate::currentDate());
    const QString kind = docType == 1
        ? tr("Supplier debt (we owe)")
        : tr("Customer debt (they owe us)");
    ui->lbContext->setText(tr("%1\n%2: %3")
                               .arg(kind,
                                    tr("Currency id"),
                                    QString::number(currencyId)));
    ui->wPartner->setSelectorName(tr("Partner"));
    ui->wPartner->selectorCallback = partnerItemSelector;
    ui->wCashbox->setSelectorName(tr("Cashbox"));
    ui->wCashbox->selectorCallback = cashboxItemSelector;

    for (int pt : kDebtRedeemPaymentTypes) {
        const char *nm = payment_names.value(pt);
        if (!nm) {
            continue;
        }
        ui->cbPaymentType->addItem(QCoreApplication::translate("PaymentType", nm), pt);
    }
    const int cashIdx = ui->cbPaymentType->findData(PAYMENT_TYPE_CASH);
    if (cashIdx >= 0) {
        ui->cbPaymentType->setCurrentIndex(cashIdx);
    }

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DlgDebtsRedeem::tryAccept);
}

DlgDebtsRedeem::~DlgDebtsRedeem()
{
    delete ui;
}

int DlgDebtsRedeem::partnerId() const
{
    return ui->wPartner->value();
}

double DlgDebtsRedeem::amount() const
{
    return ui->spAmount->getDouble();
}

int DlgDebtsRedeem::cashboxId() const
{
    return ui->wCashbox->value();
}

int DlgDebtsRedeem::paymentTypeId() const
{
    return ui->cbPaymentType->currentData().toInt();
}

QString DlgDebtsRedeem::comment() const
{
    return ui->leComment->text().trimmed();
}

QString DlgDebtsRedeem::redeemDateMysql() const
{
    return ui->deDate->date().toString(FORMAT_DATE_TO_STR_MYSQL);
}

void DlgDebtsRedeem::tryAccept()
{
    if(ui->wPartner->value() <= 0) {
        C5Message::error(tr("Select partner"));
        return;
    }
    if(ui->cbPaymentType->currentData().toInt() <= 0) {
        C5Message::error(tr("Payment type not specified"));
        return;
    }
    QDialog::accept();
}
