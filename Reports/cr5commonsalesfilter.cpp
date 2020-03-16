#include "cr5commonsalesfilter.h"
#include "ui_cr5commonsalesfilter.h"
#include "c5cache.h"

CR5CommonSalesFilter::CR5CommonSalesFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5CommonSalesFilter)
{
    ui->setupUi(this);
    ui->leState->setSelector(dbParams, ui->leStateName, cache_order_state);
    ui->leHall->setSelector(dbParams, ui->leHallName, cache_halls);
    ui->leTable->setSelector(dbParams, ui->leTableName, cache_tables);
    ui->leStaff->setSelector(dbParams, ui->leStaffName, cache_users);
    ui->leShift->setSelector(dbParams, ui->leShiftName, cache_salary_shift);
}

CR5CommonSalesFilter::~CR5CommonSalesFilter()
{
    delete ui;
}

QString CR5CommonSalesFilter::condition()
{
    QString result = " oh.f_datecash between " + ui->deStart->toMySQLDate() + " and " + ui->deEnd->toMySQLDate() + " ";
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
    return result;
}
