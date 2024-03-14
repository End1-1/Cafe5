#include "dlgregistercard.h"
#include "ui_dlgregistercard.h"
#include "dlgsearchpartner.h"

DlgRegisterCard::DlgRegisterCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgRegisterCard)
{
    ui->setupUi(this);
    ui->leCode->setFocus();
}

DlgRegisterCard::~DlgRegisterCard()
{
    delete ui;
}

void DlgRegisterCard::on_toolButton_clicked()
{
    DlgSearchPartner d;
    if (d.exec() == QDialog::Accepted) {
        ui->lePartner->setText(d.fData[2].toString());
        ui->lePhone->setText(d.fData[4].toString());
        ui->lePartner->setProperty("id", d.fData[0].toInt());
    }
}


void DlgRegisterCard::on_btnCancel_clicked()
{
    reject();
}


void DlgRegisterCard::on_btnRegister_clicked()
{
    if (ui->leCode->text().trimmed().isEmpty()) {
        C5Message::error(tr("Incorrect card code"));
        return;
    }
    if (ui->lePartner->text().isEmpty()) {
        C5Message::error(tr("Partner is not selected"));
        return;
    }
    if (ui->lePercent->getDouble() < 0.01) {
        C5Message::error(tr("Card value cannot be zero"));
        return;
    }
    C5Database db(__c5config.dbParams());
    if (ui->lePartner->property("id").toInt() == 0) {
        db[":f_name"] = ui->lePartner->text();
        db[":f_taxname"] = ui->lePartner->text();
        db[":f_phone"] = ui->lePhone->text();
        ui->lePartner->setProperty("id", db.insert("c_partners"));
    }
    db[":f_code"] = ui->leCode->text();
    db.exec("select * from b_cards_discount where f_code=:f_code");
    if (db.nextRow()) {
        C5Message::error(tr("This card already registered"));
        return;
    }
    db[":f_value"] = ui->lePercent->getDouble() / 100;
    db[":f_mode"] = 4;
    db[":f_code"] = ui->leCode->text();
    db[":f_datestart"] = QDate::currentDate();
    db[":f_dateend"] = QDate::currentDate().addDays(1000);
    db[":f_active"] = 1;
    db[":f_client"] = ui->lePartner->property("id");
    db.insert("b_cards_discount", false);
    accept();
    C5Message::info(tr("Card registered"));
}

