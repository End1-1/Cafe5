#include "ce5discountcard.h"
#include "ui_ce5discountcard.h"
#include "ce5client.h"
#include "c5cache.h"
#include "cachediscounttype.h"
#include <QDoubleValidator>

CE5DiscountCard::CE5DiscountCard(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5DiscountCard)
{
    ui->setupUi(this);
    ui->deDateEnd->setDate(QDate::currentDate().addDays(365 * 10));
    ui->leValue->setValidator(new QDoubleValidator(0, 100, 3));
    ui->leClient->setSelector(dbParams, ui->leFirstName, cache_discount_client);
    ui->leDiscount->setSelector(dbParams, ui->leDiscountName, cache_discount_type);
}

CE5DiscountCard::~CE5DiscountCard()
{
    delete ui;
}

bool CE5DiscountCard::save(QString &err, QList<QMap<QString, QVariant> > &data)
{
    C5Database db(fDBParams);
    db[":f_code"] = ui->leCard->text();
    db.exec("select * from b_cards_discount where f_code=:f_code");
    if (db.nextRow()) {
        if (ui->leCode->getInteger() != db.getInt("f_id")) {
            err += tr("Duplicate card code");
            return false;
        }
    }
    return CE5Editor::save(err, data);
}

void CE5DiscountCard::on_btnNewClient_clicked()
{
    QString id;
    if (getId<CE5Client>(id)) {
        ui->leClient->setValue(id);
    }
}

void CE5DiscountCard::on_leFirstName_textChanged(const QString &arg1)
{
    if (arg1.isEmpty()) {
        ui->leClientInfo->clear();
        ui->leLastName->clear();
        return;
    }
    C5Cache *c = C5Cache::cache(fDBParams, cache_discount_client);
    int r = c->find(ui->leClient->getInteger());
    if (r > -1) {
        ui->leLastName->setText(c->getString(r, 2));
        ui->leClientInfo->setText(c->getString(r, 3));
    }
}
