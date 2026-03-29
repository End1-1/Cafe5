#include "dlgguestinfo.h"
#include "dlgtext.h"
#include "ui_dlgguestinfo.h"

DlgGuestInfo::DlgGuestInfo(C5User *user) :
    C5Dialog(user),
    ui(new Ui::DlgGuestInfo)
{
    ui->setupUi(this);
}

DlgGuestInfo::~DlgGuestInfo()
{
    delete ui;
}

void DlgGuestInfo::setInfo(const QJsonObject &g)
{
    ui->leContactName->setText(g.value("f_guest_name").toString());
    ui->lePhoneNumber->setText(g.value("f_guest_phone").toString());
    ui->leAddress->setText(g.value("f_guest_address").toString());
}

QJsonObject DlgGuestInfo::getInfo() const
{
    return {{"f_guest_name", ui->leContactName->text()},
            {"f_guest_phone", ui->lePhoneNumber->text()},
            {"f_guest_address", ui->leAddress->text()}};
}

void DlgGuestInfo::on_btnCancel_clicked()
{
    reject();
}

void DlgGuestInfo::on_btnSave_clicked()
{
    accept();
}

void DlgGuestInfo::on_btnEditAddress_clicked()
{
    QString txt;
    if (!DlgText::getText(mUser, tr("Address"), txt)) {
        return;
    }
    ui->leAddress->setText(txt);
}

void DlgGuestInfo::on_btnEditPhone_clicked()
{
    QString txt;
    if (!DlgText::getText(mUser, tr("Phone"), txt)) {
        return;
    }
    ui->lePhoneNumber->setText(txt);
}

void DlgGuestInfo::on_btnEditContact_clicked()
{
    QString txt;
    if (!DlgText::getText(mUser, tr("Contact"), txt)) {
        return;
    }
    ui->leContactName->setText(txt);
}
