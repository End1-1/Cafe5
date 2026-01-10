#include "dlgreportsfilter.h"
#include "ui_dlgreportsfilter.h"

DlgReportsFilter::DlgReportsFilter(const QDate &d1, const QDate &d2, C5User *user)
    : C5Dialog(user),
      ui(new Ui::DlgReportsFilter)
{
    ui->setupUi(this);
    ui->date1->setDate(d1);
    ui->date2->setDate(d2);
}

DlgReportsFilter::~DlgReportsFilter() { delete ui; }

QDate DlgReportsFilter::date1()
{
    return ui->date1->date();
}

QDate DlgReportsFilter::date2()
{
    return ui->date2->date();
}

void DlgReportsFilter::on_btnOK_clicked()
{
    accept();
}

void DlgReportsFilter::on_btnCancel_clicked()
{
    reject();
}
