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

    void handleDiscount(const QJsonObject &obj);

    virtual void handleError(int err, const QString &msg);

    void tableWidgetBtnClearClicked();

    void tableWidgetBtnCalcClicked();

    void on_btnReceipt_clicked();

    void on_btnCancel_clicked();

    void on_btnCloseOrder_clicked();

    void on_btnPaymentCash_clicked();

    void on_btnPaymentCard_clicked();

    void on_btnPaymentBank_clicked();

    void on_btnPaymentOther_clicked();

    void on_btnDiscount_clicked();

private:
    Ui::DlgPayment *ui;

    C5Order *fOrder;

    void setPaymentInfo();

    void addPaymentMode(int mode, QString text, double amount);

    void checkTotal();

    bool checkForPaymentMethod(int mode, double &total);
};

#endif // DLGPAYMENT_H
