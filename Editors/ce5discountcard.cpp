#include "ce5discountcard.h"
#include "ui_ce5discountcard.h"
#include "c5cache.h"
#include "c5database.h"
#include "c5utils.h"
#include "ce5partner.h"

CE5DiscountCard::CE5DiscountCard(QWidget *parent) :
    CE5Editor(parent),
    ui(new Ui::CE5DiscountCard)
{
    ui->setupUi(this);
    ui->deDateEnd->setDate(QDate::currentDate().addDays(365 * 10));
    ui->leClient->setSelector(ui->leFirstName, cache_goods_partners);
    ui->leDiscount->setSelector(ui->leDiscountName, cache_discount_type);
    mCardTable = "b_discount_cards";
}

CE5DiscountCard::~CE5DiscountCard()
{
    delete ui;
}

bool CE5DiscountCard::isAccumulateMode(int mode) const
{
    return mode == CARD_TYPE_ACCUMULATIVE;
}

void CE5DiscountCard::applyTableForMode(int mode)
{
    mCardTable = isAccumulateMode(mode) ? "b_accumulate_cards" : "b_discount_cards";
}

void CE5DiscountCard::prepareLoad(int mode)
{
    mPendingMode = mode;
    applyTableForMode(mode);
}

QString CE5DiscountCard::table()
{
    return mCardTable.isEmpty() ? "b_discount_cards" : mCardTable;
}

void CE5DiscountCard::clear()
{
    CE5Editor::clear();
    // mPendingMode is set by prepareLoad() and consumed in setId()
    // (C5Editor::createEditor calls clear() before setId).
    mCardTable = isAccumulateMode(mPendingMode) ? "b_accumulate_cards" : "b_discount_cards";
    ui->leDiscount->setEnabled(true);
    ui->deStartDate->setDate(QDate::currentDate());
    ui->deDateEnd->setDate(QDate::currentDate().addDays(365 * 10));
    ui->checkBox->setChecked(true);
}

void CE5DiscountCard::setId(int id)
{
    const int mode = mPendingMode;
    mPendingMode = 0;

    if(id == 0) {
        mCardTable = "b_discount_cards";
        CE5Editor::setId(0);
        ui->leDiscount->setEnabled(true);
        ui->leDaysToEnd->setInteger(QDate::currentDate().daysTo(ui->deDateEnd->date()));
        return;
    }

    int loadMode = mode;

    if(loadMode == 0) {
        C5Database db;
        db[":f_id"] = id;
        db.exec("select f_mode from b_discount_cards where f_id=:f_id");

        if(db.nextRow()) {
            loadMode = db.getInt("f_mode");
        } else {
            db[":f_id"] = id;
            db.exec("select f_id from b_accumulate_cards where f_id=:f_id");

            if(db.nextRow()) {
                loadMode = CARD_TYPE_ACCUMULATIVE;
            } else {
                loadMode = CARD_TYPE_DISCOUNT;
            }
        }
    }

    applyTableForMode(loadMode);
    CE5Editor::setId(id);

    if(isAccumulateMode(loadMode)) {
        ui->leDiscount->setValue(CARD_TYPE_ACCUMULATIVE);
    }

    ui->leDiscount->setEnabled(false);
    ui->leDaysToEnd->setInteger(QDate::currentDate().daysTo(ui->deDateEnd->date()));
}

bool CE5DiscountCard::save(QString &err, QList<QMap<QString, QVariant> >& data)
{
    const int mode = ui->leDiscount->getInteger();
    applyTableForMode(mode);

    const QString code = ui->leCard->text().trimmed().replace("?", "").replace(";", "").replace(":", "");
    ui->leCard->setText(code);

    if(code.isEmpty()) {
        err += tr("Card code is empty");
        return false;
    }

    C5Database db;
    const int currentId = ui->leCode->getInteger();
    db[":f_code"] = code;
    db.exec("select f_id from b_discount_cards where f_code=:f_code");

    if(db.nextRow()) {
        if(isAccumulateMode(mode) || currentId != db.getInt("f_id")) {
            err += tr("Duplicate card code");
            return false;
        }
    }

    db[":f_code"] = code;
    db.exec("select f_id from b_accumulate_cards where f_code=:f_code");

    if(db.nextRow()) {
        if(!isAccumulateMode(mode) || currentId != db.getInt("f_id")) {
            err += tr("Duplicate card code");
            return false;
        }
    }

    QVariant modeField;

    if(isAccumulateMode(mode)) {
        modeField = ui->leDiscount->property("Field");
        ui->leDiscount->setProperty("Field", QVariant());
    }

    const bool ok = CE5Editor::save(err, data);

    if(isAccumulateMode(mode)) {
        ui->leDiscount->setProperty("Field", modeField);

        if(!data.isEmpty()) {
            data[0]["f_mode"] = CARD_TYPE_ACCUMULATIVE;
            data[0]["dtname"] = ui->leDiscountName->text();
        }
    }

    return ok;
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

void CE5DiscountCard::on_btnSetDays_clicked()
{
    ui->deDateEnd->setDate(ui->deStartDate->date().addDays(ui->leDaysToEnd->getInteger()));
}
