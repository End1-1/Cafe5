#include "dlgshopcustomer.h"
#include "c5user.h"
#include "ninterface.h"
#include "ui_dlgshopcustomer.h"

DlgShopCustomer::DlgShopCustomer(C5User *user)
    : C5ShopDialog(user)
    , ui(new Ui::DlgShopCustomer)
{
    ui->setupUi(this);
}

void DlgShopCustomer::setTin(const QString &tin)
{
    ui->leTin->setText(tin);
}

DlgShopCustomer::~DlgShopCustomer()
{
    delete ui;
}

void DlgShopCustomer::on_btnSave_clicked()
{
    NInterface::query1("/engine/v2/common/partners/save",
                       mUser->mSessionKey,
                       this,
                       {{"f_category", 2},
                        {"f_state", 1},
                        {"f_taxname", ui->leLegalName->text()},
                        {"f_name", ui->leName->text()},
                        {"f_phone", ui->lePhone->text()},
                        {"f_contact", ui->leContact->text()}},
                       [this](const QJsonObject &jo) {
                           mPartner = JsonParser<PartnerItem>::fromJson(
                               jo.value("partner").toObject());
                           accept();
                       });
}

void DlgShopCustomer::on_btnCancel_clicked()
{
    reject();
}
