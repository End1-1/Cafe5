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

    virtual void closeEvent(QCloseEvent *e);

private:
    Ui::C5SaleFromStoreOrder *ui;

    QStringList fDeleteRows;

    void loadOrder(const QString &id);

    void countTotal();

    void saveChanges();

private slots:
    void tblContextMenu(const QPoint &point);

    void actionDelete(bool v);
};

#endif // C5SALEFROMSTOREORDER_H
