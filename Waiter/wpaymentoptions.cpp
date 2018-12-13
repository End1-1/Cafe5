#include "wpaymentoptions.h"
#include "ui_wpaymentoptions.h"

WPaymentOptions::WPaymentOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WPaymentOptions)
{
    ui->setupUi(this);
}

WPaymentOptions::~WPaymentOptions()
{
    delete ui;
}

void WPaymentOptions::on_btnRemove_clicked()
{
    emit clearClicked();
}

void WPaymentOptions::on_btnCalc_clicked()
{
    emit calc();
}
