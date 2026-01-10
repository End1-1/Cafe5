#ifndef C5SALEFROMSTOREORDER_H
#define C5SALEFROMSTOREORDER_H

#include "c5dialog.h"

namespace Ui
{
class C5SaleFromStoreOrder;
}

class C5SaleFromStoreOrder : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5SaleFromStoreOrder(C5User *user);

    ~C5SaleFromStoreOrder();

    static void openOrder(C5User *user, const QString &id);

private slots:
    void removeOrderResponse(const QJsonObject &jdoc);

    void on_btnRemove_clicked();

    void on_btnPrintTax_clicked();

    void on_btnSave_clicked();

    void on_btnCopyUUID_clicked();

    void on_btnPrintA4_clicked();

    void on_btnExportToExcel_clicked();

    void on_btnPrintA4_3_clicked();

    void on_btnPrintA4_2_clicked();

private:
    Ui::C5SaleFromStoreOrder *ui;

    void loadOrder(const QString &id);

    void exportToAS(int doctype);
};

#endif // C5SALEFROMSTOREORDER_H
