#include "dlgstoreinputxmlpartnerupdate.h"
#include "c5message.h"
#include "ui_dlgstoreinputxmlpartnerupdate.h"
#include <QDialogButtonBox>

DlgStoreInputXmlPartnerUpdate::DlgStoreInputXmlPartnerUpdate(const QJsonObject &partner,
                                                             const QString &importTaxName,
                                                             const QString &importAddress,
                                                             QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgStoreInputXmlPartnerUpdate)
    , mPartner(partner)
{
    ui->setupUi(this);
    ui->lbTaxNameDb->setText(partner.value(QStringLiteral("f_taxname")).toString());
    ui->lbTaxNameXml->setText(importTaxName);
    ui->lbAddressDb->setText(partner.value(QStringLiteral("f_address")).toString());
    ui->lbAddressXml->setText(importAddress);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DlgStoreInputXmlPartnerUpdate::tryAccept);
}

DlgStoreInputXmlPartnerUpdate::~DlgStoreInputXmlPartnerUpdate()
{
    delete ui;
}

QJsonObject DlgStoreInputXmlPartnerUpdate::updatedPartner() const
{
    return mPartner;
}

void DlgStoreInputXmlPartnerUpdate::tryAccept()
{
    if (ui->chUpdateTaxName->isChecked()) {
        mPartner.insert(QStringLiteral("f_taxname"), ui->lbTaxNameXml->text());
    }
    if (ui->chUpdateAddress->isChecked()) {
        mPartner.insert(QStringLiteral("f_address"), ui->lbAddressXml->text());
    }
    if (!ui->chUpdateTaxName->isChecked() && !ui->chUpdateAddress->isChecked()) {
        C5Message::error(tr("Select at least one field to update"));
        return;
    }
    accept();
}
