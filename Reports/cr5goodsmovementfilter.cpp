#include "cr5goodsmovementfilter.h"
#include "ui_cr5goodsmovementfilter.h"

CR5GoodsMovementFilter::CR5GoodsMovementFilter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CR5GoodsMovementFilter)
{
    ui->setupUi(this);
}

CR5GoodsMovementFilter::~CR5GoodsMovementFilter()
{
    delete ui;
}
