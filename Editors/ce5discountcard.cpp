#include "ce5discountcard.h"
#include "ui_ce5discountcard.h"
#include "c5cache.h"
#include "c5database.h"
#include "ce5partner.h"
#include <QDoubleValidator>

CE5DiscountCard::CE5DiscountCard(QWidget *parent) :
    CE5Editor(parent),
    ui(new Ui::CE5DiscountCard)
{
    ui->setupUi(this);
    ui->deDateEnd->setDate(QDate::currentDate().addDays(365 * 10));
    // ui->leValue->setValidator(new QDoubleValidator(-10, 100, 3));
    ui->leClient->setSelector(ui->leFirstName, cache_goods_partners);
    ui->leDiscount->setSelector(ui->leDiscountName, cache_discount_type);
}

CE5DiscountCard::~CE5DiscountCard()
{
    delete ui;
}

bool CE5DiscountCard::save(QString &err, QList<QMap<QString, QVariant> >& data)
{
    C5Database db;
    db[":f_code"] = ui->leCard->text();
    db.exec("select * from b_cards_discount where f_code=:f_code");

    if(db.nextRow()) {
        if(ui->leCode->getInteger() != db.getInt("f_id")) {
            err += tr("Duplicate card code");
            return false;
        }
    }

    //kutakain
    if(ui->leDiscount->getInteger() == 4) {
        db[":f_code"] = ui->leCard->text();
        db.exec("select * from b_gift_card where f_code=:f_code");

        if(db.nextRow()) {
            int id = db.getInt("f_id");
            db[":f_costumer"] = ui->leClient->getInteger();
            db.update("b_gift_card", "f_code", ui->leCard->text());
            db[":f_id"] = id;
            db.exec("select * from b_gift_card_history where f_card=:f_card");

            if(!db.nextRow()) {
                db[":f_card"] = id;
                db[":f_amount"] = 0;
                db.insert("b_gift_card_history");
            }
        } else {
            db[":f_code"] = ui->leCard->text();
            db[":f_costumer"] = ui->leClient->getInteger();
            db[":f_dateregistered"] = QDate::currentDate();
            int id = db.insert("b_gift_card");
            db[":f_card"] = id;
            db[":f_amount"] = 0;
            db.insert("b_gift_card_history");
        }
    }

    return CE5Editor::save(err, data);
}

void CE5DiscountCard::on_btnNewClient_clicked()
{
    QString id;

    if(getId<CE5Partner>(id)) {
        ui->leClient->setValue(id);
    }
}

void CE5DiscountCard::on_leFirstName_textChanged(const QString &arg1)
{
    if(arg1.isEmpty()) {
        ui->leClientInfo->clear();
        return;
    }

    C5Cache *c = C5Cache::cache(cache_goods_partners);
    int r = c->find(ui->leClient->getInteger());

    if(r > -1) {
        ui->leClientInfo->setText(c->getString(r, 3));
    }
}

void CE5DiscountCard::on_leCard_returnPressed()
{
    ui->leCard->setText(ui->leCard->text().replace("?", "").replace(";", "").replace(":", ""));
}
