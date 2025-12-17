#include "dlgtimeorder.h"
#include "ui_dlgtimeorder.h"

DlgTimeorder::DlgTimeorder() :
    C5Dialog(),
    ui(new Ui::DlgTimeorder)
{
    ui->setupUi(this);
}

DlgTimeorder::~DlgTimeorder()
{
    delete ui;
}

bool DlgTimeorder::timeOrder(int& order)
{
    DlgTimeorder d;

    if(d.exec() == QDialog::Accepted) {
        order = d.fOrder;
        return true;
    }

    return false;
}

void DlgTimeorder::on_btnCancel_clicked()
{
    reject();
}

void DlgTimeorder::on_btnTime1_clicked()
{
    fOrder = 1;
    accept();
}

void DlgTimeorder::on_btnTime2_clicked()
{
    fOrder = 2;
    accept();
}

void DlgTimeorder::on_btnTime3_clicked()
{
    fOrder = 3;
    accept();
}
