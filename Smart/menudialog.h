#ifndef MENUDIALOG_H
#define MENUDIALOG_H

#include "c5dialog.h"

namespace Ui {
class MenuDialog;
}

class MenuDialog : public C5Dialog
{
    Q_OBJECT

public:
    explicit MenuDialog();
    ~MenuDialog();

private slots:
    void on_btnExit_clicked();

    void on_btnFiscalZReport_clicked();

    void on_btnReturnFiscalReceipt_clicked();

    void on_btnReportByOrder_clicked();

    void on_btnBack_clicked();

    void on_btnPrintReport_clicked();

private:
    Ui::MenuDialog *ui;
};

#endif // MENUDIALOG_H
