#ifndef DLGPAYMENT_H
#define DLGPAYMENT_H

#include "c5dialog.h"
#include "c5waiterorderdoc.h"

#define PAYDLG_NONE 0
#define PAYDLG_ORDER_CLOSE 1

namespace Ui {
class DlgPayment;
}

class C5User;

class DlgPayment : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgPayment();

    ~DlgPayment();

    static int payment(C5User *user, C5WaiterOrderDoc *order);

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

    void on_btnPayComplimentary_clicked();

    void on_btnPayTransferToRoom_clicked();

    void on_btnReceiptLanguage_clicked();

    void on_btnTax_clicked();

    void on_btnPayCityLedger_clicked();

    void on_btnSelfCost_clicked();

    void on_btnBill_clicked();

private:
    Ui::DlgPayment *ui;

    C5WaiterOrderDoc *fOrder;

    C5User *fUser;

    void setPaymentInfo();

    void addPaymentMode(int mode, QString text, double amount);

    void checkTotal();

    bool checkForPaymentMethod(int mode, double &total);

    void setLangIcon();

    void setRoomComment();

    void setCLComment();

    void setComplimentary();

    void setTaxState();

    void clearOther();

    void setSelfCost();
};

#endif // DLGPAYMENT_H
