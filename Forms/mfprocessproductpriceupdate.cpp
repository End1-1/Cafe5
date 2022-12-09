#include "mfprocessproductpriceupdate.h"
#include "ui_mfprocessproductpriceupdate.h"

MFProcessProductPriceUpdate::MFProcessProductPriceUpdate(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MFProcessProductPriceUpdate)
{
    ui->setupUi(this);
}

MFProcessProductPriceUpdate::~MFProcessProductPriceUpdate()
{
    delete ui;
}

QDate MFProcessProductPriceUpdate::date1() const
{
    return ui->leDate1->date();
}

QDate MFProcessProductPriceUpdate::date2() const
{
    return ui->leDate2->date();
}

void MFProcessProductPriceUpdate::on_btnUpdatePrices_clicked()
{
    emit startUpdate();
}
