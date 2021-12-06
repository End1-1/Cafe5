#include "cr5commonsalesfilter.h"
#include "ui_cr5commonsalesfilter.h"
#include "c5cache.h"
#include "cr5saleflags.h"

CR5CommonSalesFilter::CR5CommonSalesFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5CommonSalesFilter)
{
    ui->setupUi(this);
    ui->leState->setSelector(dbParams, ui->leStateName, cache_order_state).setMultiselection(true);
    ui->leHall->setSelector(dbParams, ui->leHallName, cache_halls).setMultiselection(true);
    ui->leTable->setSelector(dbParams, ui->leTableName, cache_tables).setMultiselection(true);
    ui->leStaff->setSelector(dbParams, ui->leStaffName, cache_users).setMultiselection(true);
    ui->leShift->setSelector(dbParams, ui->leShiftName, cache_salary_shift).setMultiselection(true);
    ui->leBuyer->setSelector(dbParams, ui->leBuyerName, cache_goods_partners).setMultiselection(true);
    ui->dt1->setDateTime(QDateTime::fromString(QDate::currentDate().toString("dd.MM.yyyy") + " 00:00", "dd.MM.yyyy HH:mm"));
    ui->dt2->setDateTime(QDateTime::fromString(QDate::currentDate().addDays(1).toString("dd.MM.yyyy") + " 00:00", "dd.MM.yyyy HH:mm"));
}

CR5CommonSalesFilter::~CR5CommonSalesFilter()
{
    delete ui;
}

QString CR5CommonSalesFilter::condition()
{
    QString result;
    if (ui->chUseClosingDateTime->isChecked()) {
        result = QString(" cast(concat(oh.f_dateclose, ' ', oh.f_timeclose) as datetime) between '%1' and '%2'")
                .arg(ui->dt1->dateTime().toString(FORMAT_DATETIME_TO_STR_MYSQL))
                .arg(ui->dt2->dateTime().toString(FORMAT_DATETIME_TO_STR_MYSQL));
    } else {
        result = " oh.f_datecash between " + ui->deStart->toMySQLDate() + " and " + ui->deEnd->toMySQLDate() + " ";
    }
    if (!ui->leState->isEmpty()) {
        result += " and oh.f_state in (" + ui->leState->text() + ") ";
    }
    if (!ui->leHall->isEmpty()) {
        result += " and oh.f_hall in (" + ui->leHall->text() + ") ";
    }
    if (!ui->leTable->isEmpty()) {
        result += " and oh.f_table in (" + ui->leTable->text() + ") ";
    }
    if (!ui->leStaff->isEmpty()) {
        result += " and oh.f_staff in (" + ui->leStaff->text() + ") ";
    }
    if (!ui->leShift->isEmpty()) {
        result += " and oh.f_shift in (" + ui->leShift->text() + ") ";
    }
    if (ui->rbRIYes->isChecked()) {
        result += " and oh.f_amounttotal<0 ";
    }
    if (ui->rbRINo->isChecked()) {
        result += " and oh.f_amounttotal>=0 ";
    }
    if (!ui->leBuyer->isEmpty()) {
        result += " and oh.f_partner in (" + ui->leBuyer->text() + ") ";
    }
    if (ui->rbTaxYes->isChecked()) {
        result += " and ot.f_receiptnumber>0 ";
    }
    if (ui->rbTaxNo->isChecked()) {
        result += " and (ot.f_receiptnumber=0 or ot.f_receiptnumber is null) ";
    }
    result += fFlags;
    return result;
}

QDate CR5CommonSalesFilter::date1() const
{
    return ui->deStart->date();
}

QDate CR5CommonSalesFilter::date2() const
{
    return ui->deEnd->date();
}

void CR5CommonSalesFilter::on_btnFlags_clicked()
{
    CR5SaleFlags f(fDBParams);
    if (f.exec() == QDialog::Accepted) {
        fFlags = f.flagsCond();
    }
}
void CR5CommonSalesFilter::on_chUseClosingDateTime_clicked(bool checked)
{
    ui->dt1->setEnabled(checked);
    ui->dt2->setEnabled(checked);
}
