#include "c5goodspricing.h"
#include "ui_c5goodspricing.h"

C5GoodsPricing::C5GoodsPricing(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5GoodsPricing)
{
    ui->setupUi(this);
}

C5GoodsPricing::~C5GoodsPricing()
{
    delete ui;
}
