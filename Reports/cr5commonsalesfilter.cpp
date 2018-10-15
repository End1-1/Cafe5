#include "cr5commonsalesfilter.h"
#include "ui_cr5commonsalesfilter.h"

CR5CommonSalesFilter::CR5CommonSalesFilter(QWidget *parent) :
    C5FilterWidget(parent),
    ui(new Ui::CR5CommonSalesFilter)
{
    ui->setupUi(this);
}

CR5CommonSalesFilter::~CR5CommonSalesFilter()
{
    delete ui;
}

QString CR5CommonSalesFilter::condition()
{
    return "";
}
