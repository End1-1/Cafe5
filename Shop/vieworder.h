#ifndef VIEWORDER_H
#define VIEWORDER_H

#include "c5shopdialog.h"

namespace Ui
{
class ViewOrder;
}

class C5Database;
class Working;

class ViewOrder : public C5ShopDialog
{
    Q_OBJECT

public:
    explicit ViewOrder(Working *w, const QString &order, C5User *user);

    ~ViewOrder();

    void printCheckWithTax(const QString &id, std::function<void(const QString &)> funcSuccess);

private slots:
    void on_btnReturn_clicked();

    void on_btnTaxReturn_clicked();

    void on_btnClose_clicked();

    void on_btnCopyUUID_clicked();

    void on_btnEditDeliveryMan_clicked();

    void on_btnEditSaler_clicked();

    void on_btnEditBuyer_clicked();

    void on_btnPrintReceipt_clicked();

    void on_btnPrintFiscal_clicked();

    void on_btnPrintReceiptA4_clicked();

    void on_btnMakeDraft_clicked();

    void on_btnSaveReturn_clicked();

    void on_tbl_cellClicked(int row, int column);

    void on_btnEditReason_clicked();

    void on_btnPrintPrices_clicked();

private:
    Ui::ViewOrder *ui;

    QString fSaleDoc;

    QString fUuid;

    Working* fWorking;

    void countOrder();
};

#endif // VIEWORDER_H
