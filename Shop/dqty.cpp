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

double DQty::getQty(const QString &title, double max, QWidget *parent)
{
    DQty *d = new DQty(parent);
    d->ui->lbTitle->setText(title);
    d->mMax = max;
    double result = 0;
    if (d->exec() == QDialog::Accepted) {
        result = d->ui->leQty->text().toDouble();
    } else {
        result = -1;
    }
    delete d;
    return result;
}

void DQty::on_leQty_returnPressed()
{
    accept();
}

void DQty::on_leQty_textChanged(const QString &arg1)
{
    if (arg1.toDouble() > mMax && mMax > 0.001) {
        ui->leQty->setDouble(mMax);
    }
}
