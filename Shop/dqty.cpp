#include "dqty.h"
#include "ui_dqty.h"
#include <QDoubleValidator>

DQty::DQty(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DQty)
{
    ui->setupUi(this);
    ui->leQty->setValidator(new QDoubleValidator(0, 1000000000, 3));
    adjustSize();
}

DQty::~DQty()
{
    delete ui;
}

double DQty::getQty(const QString &title, QWidget *parent)
{
    DQty *d = new DQty(parent);
    d->ui->lbTitle->setText(title);
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
