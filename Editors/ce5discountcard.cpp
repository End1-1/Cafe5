#include "ce5discountcard.h"
#include "ui_ce5discountcard.h"
#include "ce5client.h"
#include "c5cache.h"

CE5DiscountCard::CE5DiscountCard(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5DiscountCard)
{
    ui->setupUi(this);
    ui->deDateEnd->setDate(QDate::currentDate().addDays(365 * 10));
    ui->leValue->setValidator(new QDoubleValidator(0, 100, 3));
    ui->leClient->setSelector(dbParams, ui->leFirstName, cache_discount_client);
}

CE5DiscountCard::~CE5DiscountCard()
{
    delete ui;
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
