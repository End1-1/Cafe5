#include "dlgdebtsredeem.h"
#include "ui_dlgdebtsredeem.h"
#include "c5codenameselectorfunctions.h"
#include "c5message.h"
#include "format_date.h"
#include <QDate>
#include <QDialogButtonBox>

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
    QDialog::accept();
}
