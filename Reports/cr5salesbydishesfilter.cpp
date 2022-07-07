#include "cr5salesbydishesfilter.h"
#include "ui_cr5salesbydishesfilter.h"
#include "c5cache.h"

CR5SalesByDishesFilter::CR5SalesByDishesFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5SalesByDishesFilter)
{
    ui->setupUi(this);
    ui->leDishState->setSelector(dbParams, ui->leStateName, cache_dish_state).setMultiselection(true);
    ui->leDishState->setValue(DISH_STATE_OK);
    ui->lePart1->setSelector(dbParams, ui->lePart1Name, cache_dish_part1).setMultiselection(true);
    ui->lePart2->setSelector(dbParams, ui->lePart2Name, cache_dish_part2).setMultiselection(true);
    ui->leStore->setSelector(dbParams, ui->leStoreName, cache_goods_store).setMultiselection(true);
    ui->dt1->setDateTime(QDateTime::fromString(QDate::currentDate().toString("dd.MM.yyyy") + " 00:00", "dd.MM.yyyy HH:mm"));
    ui->dt2->setDateTime(QDateTime::fromString(QDate::currentDate().addDays(1).toString("dd.MM.yyyy") + " 00:00", "dd.MM.yyyy HH:mm"));
}

CR5SalesByDishesFilter::~CR5SalesByDishesFilter()
{
    delete ui;
}

QString CR5SalesByDishesFilter::condition()
{
    QString result;
    if (ui->chUseCloseDateTime->isChecked()) {
        result = QString(" cast(concat(oh.f_dateclose, ' ', oh.f_timeclose) as datetime) between '%1' and '%2'")
                .arg(ui->dt1->dateTime().toString(FORMAT_DATETIME_TO_STR_MYSQL))
                .arg(ui->dt2->dateTime().toString(FORMAT_DATETIME_TO_STR_MYSQL));
    } else {
        result = QString(" oh.f_datecash between '%1' and '%2' ").arg(ui->deStart->date().toString(FORMAT_DATE_TO_STR_MYSQL)).arg(ui->deEnd->date().toString(FORMAT_DATE_TO_STR_MYSQL));
    }
    if (!ui->leDishState->isEmpty()) {
        result += QString(" and ob.f_state in (%1) ").arg(ui->leDishState->text());
    }
    if (!ui->lePart1->isEmpty()) {
        result += QString(" and dpd.f_id in (%1) ").arg(ui->lePart1->text());
    }
    if (!ui->lePart2->isEmpty()) {
        result += QString(" and d.f_part in (%1) ").arg(ui->lePart2->text());
    }
    if (!ui->leStore->isEmpty()) {
        result += QString(" and ob.f_store in (%1) ").arg(ui->leStore->text());
    }
    if (ui->rbNoRecipt->isChecked()) {
        result += QString(" and d.f_netweight<0.0001 ");
    }
    if (ui->rbWithRecipe->isChecked()) {
        result += QString(" and d.f_netweight>0.0001 ");
    }
    return result;
}

QString CR5SalesByDishesFilter::filterText()
{
    QString s = QString("%1 %2 - %3").arg(tr("Date range"), ui->deStart->text(), ui->deEnd->text());
    if (ui->chUseCloseDateTime->isChecked()) {
        s = QString("%1 %2 - %3").arg(tr("Date range"), ui->dt1->text(), ui->dt2->text());
    }
    inFilterText(s, ui->leStateName);
    inFilterText(s, ui->lePart1Name);
    inFilterText(s, ui->lePart2Name);
    inFilterText(s, ui->leStoreName);
    if (ui->rbNoRecipt->isChecked()){
        s += ", " + tr("No recipe");
    } else if (ui->rbWithRecipe->isChecked()) {
        s += ", " + tr("With recipe");
    }
    return s;
}

void CR5SalesByDishesFilter::on_chUseCloseDateTime_clicked(bool checked)
{
    ui->dt1->setEnabled(checked);
    ui->dt2->setEnabled(checked);
}
