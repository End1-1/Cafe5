#include "cr5salarybyworkersfilter.h"
#include "ui_cr5salarybyworkersfilter.h"
#include "c5cache.h"
#include "format_date.h"

CR5SalaryByWorkersFilter::CR5SalaryByWorkersFilter(QWidget *parent) :
    C5FilterWidget(parent),
    ui(new Ui::CR5SalaryByWorkersFilter)
{
    ui->setupUi(this);
    ui->lePosition->setSelector(ui->lePositionName, cache_users_groups);
    ui->leEmployee->setSelector(ui->leEmployeeName, cache_users);
}

CR5SalaryByWorkersFilter::~CR5SalaryByWorkersFilter()
{
    delete ui;
}

QString CR5SalaryByWorkersFilter::condition()
{
    return "";
}

QString CR5SalaryByWorkersFilter::replaceFitler()
{
    QString result = QString("where sp.f_date between '%1' and '%2' ").arg(ui->deStart->date().toString(FORMAT_DATE_TO_STR_MYSQL))
                     .arg(ui->deEnd->date().toString(FORMAT_DATE_TO_STR_MYSQL));

    if(!ui->leEmployee->isEmpty()) {
        result += " and sp.f_worker in (" + ui->leEmployee->text() + ") ";
    }

    if(!ui->lePosition->isEmpty()) {
        result += " and sp.f_position in (" + ui->lePosition->text() + ") ";
    }

    return result;
}
