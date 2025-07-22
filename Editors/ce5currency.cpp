#include "ce5currency.h"
#include "ui_ce5currency.h"

CE5Currency::CE5Currency(QWidget *parent) :
    CE5Editor(parent),
    ui(new Ui::CE5Currency)
{
    ui->setupUi(this);
}

CE5Currency::~CE5Currency()
{
    delete ui;
}

QString CE5Currency::title()
{
    return tr("Currency");
}

QString CE5Currency::table()
{
    return "e_currency";
}
