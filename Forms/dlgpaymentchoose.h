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

    void on_leCash_textChanged(const QString &arg1);

    void on_leCard_textChanged(const QString &arg1);

    void on_leBankTransfer_textChanged(const QString &arg1);

    void on_leIdram_textChanged(const QString &arg1);

    void on_leTelcell_textChanged(const QString &arg1);

    void on_lePrepaid_textChanged(const QString &arg1);

    void on_leDebt_textChanged(const QString &arg1);

private:
    Ui::DlgPaymentChoose *ui;

    void clearAll(QLineEdit *le);

    void countChange();
};

#endif // DLGPAYMENTCHOOSE_H