#include "dlgguestinfo.h"
#include "dlgtext.h"
#include "ui_dlgguestinfo.h"
#include <QRegularExpression>

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
    ui->lePhoneNumber->setText(displayPhone(g.value("f_guest_phone").toString()));
    ui->leAddress->setText(g.value("f_guest_address").toString());
}

QJsonObject DlgGuestInfo::getInfo() const
{
    return {{"f_guest_name", ui->leContactName->text()},
            {"f_guest_phone", normalizePhone(ui->lePhoneNumber->text())},
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
    ui->lePhoneNumber->setText(displayPhone(txt));
}

void DlgGuestInfo::on_btnEditContact_clicked()
{
    QString txt;
    if (!DlgText::getText(mUser, tr("Contact"), txt)) {
        return;
    }
    ui->leContactName->setText(txt);
}

QString DlgGuestInfo::normalizePhone(const QString &phone)
{
    QString digits = phone;
    digits.remove(QRegularExpression("[^0-9]"));

    if(digits.startsWith("374")) {
        digits.remove(0, 3);
    } else if(digits.startsWith("0")) {
        digits.remove(0, 1);
    }

    if(digits.length() > 8) {
        digits = digits.left(8);
    }

    return QString("+374%1").arg(digits);
}

QString DlgGuestInfo::displayPhone(const QString &phone)
{
    QString normalized = normalizePhone(phone);

    if(normalized.length() != 12) {
        return normalized;
    }

    const QString local = normalized.mid(4); // xxyyyyyy
    return QString("+374 %1 %2-%3")
            .arg(local.mid(0, 2))
            .arg(local.mid(2, 3))
            .arg(local.mid(5, 3));
}
