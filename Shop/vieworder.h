#ifndef VIEWORDER_H
#define VIEWORDER_H

#include "c5dialog.h"
#include "oheader.h"

namespace Ui {
class ViewOrder;
}

class C5Database;
class Working;

class ViewOrder : public C5Dialog
{
    Q_OBJECT

public:
    explicit ViewOrder(Working *w, const QString &order);

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

    void on_btnPrintReceiptA4_clicked();

    void on_btnMakeDraft_clicked();

private:
    Ui::ViewOrder *ui;

    OHeader fOHeader;

    QString fSaleDoc;

    QString fUuid;

    Working *fWorking;

    void returnFalse(const QString &msg, C5Database *db);
};

#endif // VIEWORDER_H
