#ifndef SALES_H
#define SALES_H

#include <QDialog>

namespace Ui {
class Sales;
}

class Sales : public QDialog
{
    Q_OBJECT

public:
    explicit Sales(QWidget *parent = nullptr);

    ~Sales();

    static void showSales(QWidget *parent = nullptr);

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

private:
    Ui::Sales *ui;

    void changeDate(int d);

    void refresh();

    void refreshTotal();

    void refreshItems();

    void refreshTotalItems();

    int fViewMode;

    QString userCond() const;
};

#endif // SALES_H
