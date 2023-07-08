#include "ce5currencycrossraterecord.h"
#include "ui_ce5currencycrossraterecord.h"
#include "c5cache.h"
#include <QDoubleValidator>

CE5CurrencyCrossRateRecord::CE5CurrencyCrossRateRecord(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5CurrencyCrossRateRecord)
{
    ui->setupUi(this);
    ui->leCurr->setSelector(dbParams, ui->leCurrName, cache_currency);
    ui->leCurr_2->setSelector(dbParams, ui->leCurrName_2, cache_currency);
    ui->leRate->setValidator(new QDoubleValidator(0, 999999, 7));
}

CE5CurrencyCrossRateRecord::~CE5CurrencyCrossRateRecord()
{
    delete ui;
}

QString CE5CurrencyCrossRateRecord::title()
{
    return tr("Cross rate");
}

QString CE5CurrencyCrossRateRecord::table()
{
    return "e_currency_cross_rate_history";
}

bool CE5CurrencyCrossRateRecord::save(QString &err, QList<QMap<QString, QVariant> > &data)
{
    if (!CE5Editor::save(err, data)) {
        C5Message::error(err);
        return false;
    }
    if (ui->chSetCurrentRate->isChecked()) {
        C5Database db(fDBParams);
        db[":f_date"] = ui->leDate->date();
        db[":f_currency1"] = ui->leCurr->getInteger();
        db[":f_currency2"] = ui->leCurr_2->getInteger();
        db[":f_rate"] = ui->leRate->getDouble();
        db.exec("update e_currency_cross_rate set f_rate=:f_rate where f_currency1=:f_currency1 and f_currency2=:f_currency2");
    }
    return true;
}
