#include "dlgpaymentstub.h"
#include "ui_dlgpaymentstub.h"

DlgPaymentStub::DlgPaymentStub(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgPaymentStub)
{
    ui->setupUi(this);
    setModal(true);
    setWindowFlags(windowFlags() | Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
}

DlgPaymentStub::~DlgPaymentStub()
{
    delete ui;
}

void DlgPaymentStub::finishWith(PaymentStubResult result)
{
    m_result = result;
    accept();
}

void DlgPaymentStub::on_btnPaid_clicked()
{
    finishWith(PaymentStubResult::Paid);
}

void DlgPaymentStub::on_btnInsufficient_clicked()
{
    finishWith(PaymentStubResult::InsufficientFunds);
}

void DlgPaymentStub::on_btnOtherError_clicked()
{
    finishWith(PaymentStubResult::OtherError);
}
