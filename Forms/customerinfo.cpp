#include "customerinfo.h"
#include "ui_customerinfo.h"
#include "amkbd.h"
#include "c5database.h"
#include "c5message.h"
#include "c5config.h"
#include "registercustomer.h"
#include <QEvent>
#include <QKeyEvent>

CustomerInfo::CustomerInfo(C5User *user) :
    C5Dialog(user),
    ui(new Ui::CustomerInfo)
{
    ui->setupUi(this);
    fCustomerId = 0;
    ui->leCustomer->installEventFilter(this);
    ui->leAddress->installEventFilter(this);

    if(__c5config.getRegValue("language").toString() == "us") {
        ui->btnKbdUS->setChecked(true);
        ui->btnKbdUS->click();
    } else  if(__c5config.getRegValue("language").toString() == "am") {
        ui->btnKdbAM->setChecked(true);
    } else {
        ui->btnKbdUS->setChecked(true);
    }
}

CustomerInfo::~CustomerInfo()
{
    delete ui;
}

bool CustomerInfo::getCustomer(int& id, QString &name, QString &phone, QString &address, C5User *user)
{
    CustomerInfo ci(user);
    bool result = ci.exec() == QDialog::Accepted;

    if(result) {
        name = ci.ui->leCustomer->text();
        phone = ci.ui->lePhone->text();
        address = ci.ui->leAddress->text();
        id = ci.fCustomerId;
    }

    return result;
}

bool CustomerInfo::eventFilter(QObject *o, QEvent *e)
{
    if(o == ui->leCustomer || o == ui->leAddress) {
        if(e->type() == QEvent::KeyPress) {
            QKeyEvent *ek = static_cast<QKeyEvent*>(e);

            if(ui->btnKdbAM->isChecked()) {
                if(ek->key()) {
                    int index = keys.indexOf(ek->nativeVirtualKey());

                    if(index < 0) {
                        return C5Dialog::eventFilter(o, e);
                    }

                    QString txt = chars[index];

                    if(ek->modifiers() &Qt::ShiftModifier) {
                        txt = txt.toUpper();
                    }

                    auto *ekk = new QKeyEvent(QEvent::KeyPress, chars[index].at(0).unicode(), ek->modifiers(), txt);
                    qApp->postEvent(o, ekk);
                    return true;
                    //qDebug() << ek->key() << ek->text() << ek->nativeVirtualKey();
                }
            }
        }
    }

    return C5Dialog::eventFilter(o, e);
}

void CustomerInfo::on_pushButton_2_clicked()
{
    reject();
}

void CustomerInfo::on_pushButton_clicked()
{
    if(fCustomerId > 0) {
        C5Database db;
        db[":f_taxname"] = ui->lePhone->text() + " " + ui->leCustomer->text();
        db[":f_contact"] = ui->leCustomer->text();
        db[":f_phone"] = ui->lePhone->text();
        db[":f_address"] = ui->leAddress->text();
        db.update("c_partners", "f_id", fCustomerId);
        accept();
        return;
    }

    QString err;

    if(ui->leAddress->text().isEmpty()) {
        err += tr("Address must be defined");
    }

    if(ui->lePhone->text().isEmpty()) {
        err += tr("Phone must defined");
    }

    if(err.isEmpty() == false) {
        C5Message::error(err);
        return;
    }

    C5Database db;
    db[":f_taxname"] = ui->lePhone->text() + " " + ui->leCustomer->text();
    db[":f_contact"] = ui->leCustomer->text();
    db[":f_name"] = ui->leCustomer->text();
    db[":f_phone"] = ui->lePhone->text();
    db[":f_address"] = ui->leAddress->text();
    db[":f_category"] = 1;//__c5config.getValue(param_waiter_delivery_costumer_category).toInt();

    if(fCustomerId == 0) {
        fCustomerId = db.insert("c_partners");
    } else {
        db.update("c_partnes", "f_id", fCustomerId);
    }

    if(fCustomerId == 0) {
        C5Message::error(db.fLastError);
        return;
    }

    accept();
}

void CustomerInfo::on_lePhone_returnPressed()
{
    QString phone = ui->lePhone->text();

    if(phone.length() == 12) {
        if(phone.at(3) != ' ' && phone.at(6) != '-' && phone.at(8) != '-') {
            C5Message::error(tr("Invalid phone number"));
            return;
        }
    } else if(phone.length() != 9) {
        C5Message::error(tr("Invalid phone number"));
        return;
    }

    if(phone.length() == 9) {
        phone.insert(3, " ");
        phone.insert(6, "-");
        phone.insert(9, "-");
    }

    ui->lePhone->setText(phone);
    C5Database db;
    db[":f_phone"] = phone;
    db.exec("select * from c_partners where f_phone=:f_phone");

    if(db.nextRow()) {
        fCustomerId = db.getInt("f_id");
        ui->leAddress->setText(db.getString("f_address"));
        ui->leCustomer->setText(db.getString("f_contact"));
    }
}

void CustomerInfo::on_btnKbdUS_clicked(bool checked)
{
    if(checked) {
        ui->btnKdbAM->setChecked(false);
        __c5config.setRegValue("language", "us");
    }
}

void CustomerInfo::on_btnKdbAM_clicked(bool checked)
{
    if(checked) {
        ui->btnKbdUS->setChecked(false);
        __c5config.setRegValue("language", "am");
    }
}

void CustomerInfo::on_btnExtendedVersion_clicked()
{
    RegisterCustomer(this).exec();
}
