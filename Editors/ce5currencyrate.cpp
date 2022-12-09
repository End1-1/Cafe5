#include "ce5currencyrate.h"
#include "ui_ce5currencyrate.h"
#include "c5cache.h"

CE5CurrencyRate::CE5CurrencyRate(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5CurrencyRate)
{
    ui->setupUi(this);
    ui->leCurr->setSelector(dbParams, ui->leCurrName, cache_currency);
}

CE5CurrencyRate::~CE5CurrencyRate()
{
    delete ui;
}

QString CE5CurrencyRate::title()
{
    return tr("Currency rate");
}

QString CE5CurrencyRate::table()
{
    return "e_currency_rate_history";
}

bool CE5CurrencyRate::save(QString &err, QList<QMap<QString, QVariant> > &data)
{
    if (!CE5Editor::save(err, data)) {
        C5Message::error(err);
        return false;
    }
    if (ui->leDate->date() == QDate::currentDate()) {
        C5Database db(fDBParams);
        db[":f_id"] = ui->leCurr->getInteger();
        db[":f_rate"] = ui->leRate->getDouble();
        Q_ASSERT(db.exec("update e_Currency set f_rate=:f_rate where f_id=:f_id"));
    }
    return true;
}
