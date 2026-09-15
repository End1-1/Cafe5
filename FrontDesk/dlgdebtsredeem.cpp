#include "dlgdebtsredeem.h"
#include "ui_dlgdebtsredeem.h"
#include "c5codenameselectorfunctions.h"
#include "c5message.h"
#include "dict_payment_type.h"
#include "format_date.h"
#include <QCoreApplication>
#include <QDate>
#include <QDialogButtonBox>
#include <QPushButton>

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

    ui->btnDelete->setVisible(false);
    ui->btnDelete->setStyleSheet(QStringLiteral("QPushButton { color: #b00020; }"));
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DlgDebtsRedeem::tryAccept);
    connect(ui->btnDelete, &QPushButton::clicked, this, &DlgDebtsRedeem::onDeleteClicked);
}

DlgDebtsRedeem::~DlgDebtsRedeem()
{
    delete ui;
}

void DlgDebtsRedeem::loadRedeem(const QJsonObject &redeem)
{
    mDebtId = redeem.value(QStringLiteral("debt_id")).toInt();
    setWindowTitle(tr("Edit debt payment"));
    ui->btnDelete->setVisible(true);

    const QString partnerName = redeem.value(QStringLiteral("partner_name")).toString();
    ui->wPartner->setCodeAndName(redeem.value(QStringLiteral("partner_id")).toInt(), partnerName);
    ui->wPartner->setEnabled(false);

    const QDate d = QDate::fromString(redeem.value(QStringLiteral("date")).toString(), QStringLiteral("yyyy-MM-dd"));
    if (d.isValid()) {
        ui->deDate->setDate(d);
    }
    ui->spAmount->setDouble(redeem.value(QStringLiteral("amount")).toDouble());
    ui->leComment->setText(redeem.value(QStringLiteral("comment")).toString());

    const int cashboxId = redeem.value(QStringLiteral("cashbox_id")).toInt();
    const QString cashboxName = redeem.value(QStringLiteral("cashbox_name")).toString();
    if (cashboxId > 0) {
        ui->wCashbox->setCodeAndName(cashboxId, cashboxName);
    }
    ui->wCashbox->setEnabled(false);

    const int pt = redeem.value(QStringLiteral("payment_type_id")).toInt(PAYMENT_TYPE_CASH);
    const int ptIdx = ui->cbPaymentType->findData(pt);
    if (ptIdx >= 0) {
        ui->cbPaymentType->setCurrentIndex(ptIdx);
    }
}

int DlgDebtsRedeem::debtId() const
{
    return mDebtId;
}

bool DlgDebtsRedeem::isEditMode() const
{
    return mDebtId > 0;
}

bool DlgDebtsRedeem::deleteRequested() const
{
    return mDeleteRequested;
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
    if(ui->spAmount->getDouble() <= 0.00001) {
        C5Message::error(tr("Amount must be greater than zero"));
        return;
    }
    if(ui->cbPaymentType->currentData().toInt() <= 0) {
        C5Message::error(tr("Payment type not specified"));
        return;
    }
    if(!isEditMode() && ui->wCashbox->value() <= 0) {
        // cashbox optional historically — keep optional
    }
    QDialog::accept();
}

void DlgDebtsRedeem::onDeleteClicked()
{
    if(C5Message::question(tr("Delete this debt payment?")) != QDialog::Accepted) {
        return;
    }
    mDeleteRequested = true;
    QDialog::accept();
}
