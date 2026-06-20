#ifndef DLGPAYMENTSTUB_H
#define DLGPAYMENTSTUB_H

#include "paymentstubtypes.h"

#include <QDialog>

namespace Ui {
class DlgPaymentStub;
}

class DlgPaymentStub : public QDialog
{
    Q_OBJECT

public:
    explicit DlgPaymentStub(QWidget *parent = nullptr);
    ~DlgPaymentStub() override;

    PaymentStubResult stubResult() const { return m_result; }

private slots:
    void on_btnPaid_clicked();
    void on_btnInsufficient_clicked();
    void on_btnOtherError_clicked();

private:
    void finishWith(PaymentStubResult result);

    Ui::DlgPaymentStub *ui;
    PaymentStubResult m_result = PaymentStubResult::None;
};

#endif // DLGPAYMENTSTUB_H
