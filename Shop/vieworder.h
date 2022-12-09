#ifndef VIEWORDER_H
#define VIEWORDER_H

#include "c5dialog.h"

namespace Ui {
class ViewOrder;
}

class C5Database;

class ViewOrder : public C5Dialog
{
    Q_OBJECT

public:
    explicit ViewOrder(const QString &order);

    ~ViewOrder();

private slots:
    void on_btnReturn_clicked();

    void on_btnTaxReturn_clicked();

    void on_btnClose_clicked();

    void on_btnEditDate_clicked();

    void on_btnCopyUUID_clicked();

    void on_btnEditDeliveryMan_clicked();

    void on_btnEditSaler_clicked();

    void on_btnEditBuyer_clicked();

    void on_btnPrintReceipt_clicked();

    void on_btnPrintFiscal_clicked();

    bool printCheckWithTax(C5Database &db, const QString &id, QString &rseq);

private:
    Ui::ViewOrder *ui;

    int fSaleType;

    int fPartner;

    QString fSaleDoc;

    QString fUuid;

    void returnFalse(const QString &msg, C5Database *db);
};

#endif // VIEWORDER_H
