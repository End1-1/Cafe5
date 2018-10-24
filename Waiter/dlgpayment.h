#ifndef DLGPAYMENT_H
#define DLGPAYMENT_H

#include "c5dialog.h"
#include "c5order.h"

#define PAYDLG_NONE 0
#define PAYDLG_ORDER_CLOSE 1

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
    void handleReceipt(const QJsonObject &obj);

    void handleCloseOrder(const QJsonObject &obj);

    virtual void handleError(int err, const QString &msg);

    void on_btnReceipt_clicked();

    void on_btnCash_clicked();

    void on_btnCancel_clicked();

    void on_btnClearCash_clicked();

    void on_btnClearCard_clicked();

    void on_btnClearBank_clicked();

    void on_btnClearOther_clicked();

    void on_btnSetCash_clicked();

    void on_btnSetCard_clicked();

    void on_btnSetBank_clicked();

    void on_btnSetOther_clicked();

private:
    Ui::DlgPayment *ui;

    C5Order *fOrder;

    void checkTotal();
};

#endif // DLGPAYMENT_H
