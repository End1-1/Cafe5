#ifndef MENUDIALOG_H
#define MENUDIALOG_H

#include "c5dialog.h"

namespace Ui {
class MenuDialog;
}

class C5User;

class MenuDialog : public C5Dialog
{
    Q_OBJECT

public:
    explicit MenuDialog(C5User *u);
    ~MenuDialog();

private slots:
    void on_btnExit_clicked();

    void on_btnFiscalZReport_clicked();

    void on_btnReturnFiscalReceipt_clicked();

    void on_btnReportByOrder_clicked();

    void on_btnBack_clicked();

    void on_btnPrintReport_clicked();

    void on_btnCashin_clicked();

    void on_btnCashout_clicked();

    void on_btnCloseSession_clicked();

    void on_btnSessoinMoney_clicked();

private:
    Ui::MenuDialog *ui;

    C5User *fUser;
};

#endif // MENUDIALOG_H
