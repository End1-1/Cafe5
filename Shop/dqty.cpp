#include "dqty.h"
#include "ui_dqty.h"

DQty::DQty(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DQty)
{
    ui->setupUi(this);
    ui->leQty->setValidator(new QDoubleValidator(0, 1000000000, 3));
}

DQty::~DQty()
{
    delete ui;
}

double DQty::getQty(QWidget *parent)
{
    DQty *d = new DQty(parent);
    double result = 0;
    if (d->exec() == QDialog::Accepted)
        result = d->ui->leQty->text().toDouble();
    delete d;
    return result;
}

void DQty::on_leQty_returnPressed()
{
    accept();
}
