#include "cr5reportsfilter.h"
#include "ui_cr5reportsfilter.h"

CR5ReportsFilter::CR5ReportsFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5ReportsFilter)
{
    ui->setupUi(this);
}


CR5ReportsFilter::~CR5ReportsFilter()
{
    delete ui;
}

QString CR5ReportsFilter::condition()
{
    return "";
}

QString CR5ReportsFilter::d1()
{
    return ui->leDate1->toMySQLDate();
}

QString CR5ReportsFilter::d2()
{
    return ui->leDate2->toMySQLDate();
}
