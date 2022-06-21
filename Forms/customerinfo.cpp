#include "customerinfo.h"
#include "ui_customerinfo.h"

CustomerInfo::CustomerInfo() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::CustomerInfo)
{
    ui->setupUi(this);
    fCustomerId = 0;
}

CustomerInfo::~CustomerInfo()
{
    delete ui;
}

bool CustomerInfo::getCustomer(int &id, QString &name, QString &phone, QString &address)
{
    CustomerInfo ci;
    bool result = ci.exec() == QDialog::Accepted;
    if (result) {
        name = ci.ui->leCustomer->text();
        phone = ci.ui->lePhone->text();
        address = ci.ui->leAddress->text();
        id = ci.fCustomerId;
    }
    return result;
}

void CustomerInfo::on_pushButton_2_clicked()
{
    reject();
}

void CustomerInfo::on_pushButton_clicked()
{
    if (fCustomerId > 0) {
        C5Database db(fDBParams);
        db[":f_taxname"] = ui->lePhone->text() + " " + ui->leCustomer->text();
        db[":f_contact"] = ui->leCustomer->text();
        db[":f_phone"] = ui->lePhone->text();
        db[":f_address"] = ui->leAddress->text();
        db.update("c_partners", "f_id", fCustomerId);
        accept();
        return;
    }
    QString err;
    if (ui->leAddress->text().isEmpty()) {
        err += tr("Address must be defined");
    }
    if (ui->lePhone->text().isEmpty()) {
        err += tr("Phone must defined");
    }
    if (err.isEmpty() == false) {
        C5Message::error(err);
        return;
    }
    C5Database db(fDBParams);
    db[":f_taxname"] = ui->lePhone->text() + " " + ui->leCustomer->text();
    db[":f_contact"] = ui->leCustomer->text();
    db[":f_phone"] = ui->lePhone->text();
    db[":f_address"] = ui->leAddress->text();
    if (fCustomerId == 0) {
        fCustomerId = db.insert("c_partners");
    } else {
        db.update("c_partnes", "f_id", fCustomerId);
    }
    if (fCustomerId == 0) {
        C5Message::error(db.fLastError);
        return;
    }
    accept();
}

void CustomerInfo::on_lePhone_textEdited(const QString &arg1)
{

}

void CustomerInfo::on_lePhone_returnPressed()
{
    QString phone = ui->lePhone->text();
    if (phone.length() == 12){
        if (phone.at(3) != ' ' && phone.at(6) != '-' && phone.at(8) != '-') {
            C5Message::error(tr("Invalid phone number"));
            return;
        }
    } else if (phone.length() != 9) {
        C5Message::error(tr("Invalid phone number"));
        return;
    }
    if (phone.length() == 9){
        phone.insert(3, " ");
        phone.insert(6, "-");
        phone.insert(9, "-");
    }
    ui->lePhone->setText(phone);
    C5Database db(fDBParams);
    db[":f_phone"] = phone;
    db.exec("select * from c_partners where f_phone=:f_phone");
    if (db.nextRow()) {
        fCustomerId = db.getInt("f_id");
        ui->leAddress->setText(db.getString("f_address"));
        ui->leCustomer->setText(db.getString("f_contact"));
    }
}
