#ifndef DLGPAYMENT_H
#define DLGPAYMENT_H

#include "c5dialog.h"
#include "c5order.h"

namespace Ui {
class DlgPayment;
}

class DlgPayment : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgPayment(QWidget *parent = 0);

    ~DlgPayment();

    static int payment(C5Order *order);

private slots:
    void on_btnPrepaymentCash_2_clicked();

private:
    Ui::DlgPayment *ui;

    C5Order *fOrder;
};

#endif // DLGPAYMENT_H
