#ifndef SALES_H
#define SALES_H

#include "c5dialog.h"

namespace Ui {
class Sales;
}

class Sales : public C5Dialog
{
    Q_OBJECT

public:
    explicit Sales();

    ~Sales();

    static void showSales();

private slots:
    void on_btnDateLeft_clicked();

    void on_btnDateRight_clicked();

    void on_btnPrint_clicked();

    void on_btnItemBack_clicked();

    void on_btnRefresh_clicked();

    void on_btnModeTotal_clicked();

    void on_btnModeItems_clicked();

    void on_btnTotalByItems_clicked();

    void on_btnPrintTax_clicked();

    void on_btnRetryUpload_clicked();

    void on_btnCashColletion_clicked();

    void on_leFilter_textChanged(const QString &arg1);

    void on_btnGroups_clicked();

    void on_btnChangeDate_clicked();

private:
    Ui::Sales *ui;

    void changeDate(int d);

    void refresh();

    void refreshTotal();

    void refreshItems();

    void refreshTotalItems();

    void refreshGroups();

    int fViewMode;

    QString userCond() const;

    void printpreview();

    int sumOfColumnsWidghtBefore(int column);
};

#endif // SALES_H
