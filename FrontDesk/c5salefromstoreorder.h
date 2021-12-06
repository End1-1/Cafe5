#ifndef C5SALEFROMSTOREORDER_H
#define C5SALEFROMSTOREORDER_H

#include "c5dialog.h"

namespace Ui {
class C5SaleFromStoreOrder;
}

class C5SaleFromStoreOrder : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5SaleFromStoreOrder(const QStringList &dbParams);

    ~C5SaleFromStoreOrder();

    static void openOrder(const QStringList &dbParams, const QString &id);

private slots:
    void on_btnRemove_clicked();

    void on_btnPrintTax_clicked();

    void on_btnSave_clicked();

    void on_btnCopyUUID_clicked();

private:
    Ui::C5SaleFromStoreOrder *ui;

    void loadOrder(const QString &id);
};

#endif // C5SALEFROMSTOREORDER_H
