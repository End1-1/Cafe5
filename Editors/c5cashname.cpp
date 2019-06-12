#include "c5cashname.h"
#include "ui_c5cashname.h"

C5CashName::C5CashName(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::C5CashName)
{
    ui->setupUi(this);
}

C5CashName::~C5CashName()
{
    delete ui;
}

QString C5CashName::title()
{
    return tr("Cash name");
}

QString C5CashName::table()
{
    return "e_cash_names";
}
