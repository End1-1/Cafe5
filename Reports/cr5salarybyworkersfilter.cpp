#include "cr5salarybyworkersfilter.h"
#include "ui_cr5salarybyworkersfilter.h"
#include "c5cache.h"

CR5SalaryByWorkersFilter::CR5SalaryByWorkersFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5SalaryByWorkersFilter)
{
    ui->setupUi(this);
    ui->leShift->setSelector(dbParams, ui->leShiftName, cache_salary_shift);
    ui->leEmployee->setSelector(dbParams, ui->leEmployeeName, cache_users);
}

CR5SalaryByWorkersFilter::~CR5SalaryByWorkersFilter()
{
    delete ui;
}

QString CR5SalaryByWorkersFilter::condition()
{
    QString result = QString(" h.f_date between '%1' and '%2' ").arg(ui->deStart->date().toString(FORMAT_DATE_TO_STR_MYSQL))
            .arg(ui->deEnd->date().toString(FORMAT_DATE_TO_STR_MYSQL));
    if (!ui->leEmployee->isEmpty()) {
        result += " and sb.f_user in (" + ui->leEmployee->text() + ") ";
    }
    if (!ui->leShift->isEmpty()) {
        result += " and so.f_shift in (" + ui->leShift->text() + ") ";
    }
    return result;
}
