#include "cr5salefromstoretotalfilter.h"
#include "ui_cr5salefromstoretotalfilter.h"

CR5SaleFromStoreTotalFilter::CR5SaleFromStoreTotalFilter(const QStringList &dbParams, QWidget *parent) :
    C5FilterWidget(dbParams, parent),
    ui(new Ui::CR5SaleFromStoreTotalFilter)
{
    ui->setupUi(this);
}

CR5SaleFromStoreTotalFilter::~CR5SaleFromStoreTotalFilter()
{
    delete ui;
}

QString CR5SaleFromStoreTotalFilter::condition()
{
    return "";
}
