#include "dlgpayment.h"
#include "ui_dlgpayment.h"

DlgPayment::DlgPayment(QWidget *parent) :
    C5Dialog(parent),
    ui(new Ui::DlgPayment)
{
    ui->setupUi(this);
}

DlgPayment::~DlgPayment()
{
    delete ui;
}

int DlgPayment::payment(C5Order *order)
{
    DlgPayment *d = new DlgPayment(C5Config::fParentWidget);
    d->fOrder = order;
    d->showFullScreen();
    d->hide();
    int result = d->exec();
    delete d;
    return result;
}

void DlgPayment::on_btnPrepaymentCash_2_clicked()
{
    reject();
}
