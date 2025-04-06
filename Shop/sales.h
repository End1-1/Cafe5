#ifndef SALES_H
#define SALES_H

#include "c5dialog.h"
#include "c5database.h"

namespace Ui
{
class Sales;
}

class C5User;
class Working;

class Sales : public C5Dialog
{
    Q_OBJECT

public:
    explicit Sales(C5User *user);

    ~Sales();

    static void showSales(Working *w, C5User *u);

    static bool printCheckWithTax(C5Database &db, const QString &id);

    static bool printReceipt(const QString &id);

private slots:
    void on_btnDateLeft_clicked();

    void on_btnDateRight_clicked();

    void on_btnItemBack_clicked();

    void on_btnRefresh_clicked();

    void on_btnModeTotal_clicked();

    void on_btnModeItems_clicked();

    void on_btnTotalByItems_clicked();

    void on_btnCashColletion_clicked();

    void on_leFilter_textChanged(const QString &arg1);

    void on_btnGroups_clicked();

    void on_btnPrintTaxZ_clicked();

    void on_btnPrintTaxX_clicked();

    void on_btnExit_clicked();

    void on_btnViewOrder_clicked();

    void on_btnChangeDate_clicked();

    void on_deStart_textChanged(const QString &arg1);

    void on_deEnd_textChanged(const QString &arg1);

    void on_btnItemChange_clicked();

private:
    Ui::Sales *ui;

    C5User *fUser;

    Working *fWorking;

    void changeDate(int d);

    void refresh();

    void refreshTotal();

    void refreshItems();

    void refreshTotalItems();

    void refreshGroups();

    int fViewMode;

    void printpreview();

    void printTaxReport(int report_type);

    int sumOfColumnsWidghtBefore(int column);
};

#endif // SALES_H
