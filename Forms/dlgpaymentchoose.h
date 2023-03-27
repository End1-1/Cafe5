#ifndef DLGPAYMENTCHOOSE_H
#define DLGPAYMENTCHOOSE_H

#include "c5dialog.h"

namespace Ui {
class DlgPaymentChoose;
}

class QLineEdit;

class DlgPaymentChoose : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgPaymentChoose(const QStringList &dbParams);
    ~DlgPaymentChoose();
    static bool getValues(double total, double &cash, double &card, double &idram, double &telcell, double &bank,
                          double &prepaid, double &debt, double &cashin, double &change, bool &fiscal);

private slots:

    void on_btnBack_clicked();

    void on_btnCash_clicked();

    void on_btnCard_clicked();

    void on_btnIdram_clicked();

    void on_btnTelcell_clicked();

    void on_btnPrepaid_clicked();

    void on_btnDebt_clicked();

    void on_leCashIn_textChanged(const QString &arg1);

    void on_btnBankTransfer_clicked();

    void on_btnPay_clicked();

private:
    Ui::DlgPaymentChoose *ui;

    void clearAll(QLineEdit *le);
};

#endif // DLGPAYMENTCHOOSE_H
