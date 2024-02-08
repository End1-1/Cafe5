#include "cr5consuptionbysalesqty.h"
#include "ui_cr5consuptionbysalesqty.h"

cr5consuptionbysalesqty::cr5consuptionbysalesqty(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::cr5consuptionbysalesqty)
{
    ui->setupUi(this);
    ui->leQty->selectAll();
}

cr5consuptionbysalesqty::~cr5consuptionbysalesqty()
{
    delete ui;
}

cr5consuptionbysalesqty::ResultType cr5consuptionbysalesqty::qty(QWidget *parent, double &qty)
{
    cr5consuptionbysalesqty d(parent);
    d.exec();
    qty = d.ui->leQty->value();
    return d.fResultType;
}

void cr5consuptionbysalesqty::on_btnCancel_clicked()
{
    fResultType = rtCancel;
    accept();
}


void cr5consuptionbysalesqty::on_btnAdd_clicked()
{
    fResultType = rtAdd;
    accept();
}


void cr5consuptionbysalesqty::on_btnClear_clicked()
{
    fResultType = rtClear;
    accept();
}


void cr5consuptionbysalesqty::on_btnOk_clicked()
{
    fResultType = rtOk;
    accept();
}

