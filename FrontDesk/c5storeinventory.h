#ifndef C5STOREINVENTORY_H
#define C5STOREINVENTORY_H

#include "c5officewidget.h"
#include "semiready_batch.h"
#include "store_inventory_document.h"
#include <QVector>

namespace Ui
{
class C5StoreInventory;
}

class C5TableWidget;

class C5StoreInventory : public C5OfficeWidget
{
    Q_OBJECT

    enum Columns { col_group_name = 0, col_goods_id, col_goods_name, col_qty_user, col_qty_historical, col_qty_diff, col_unit, col_price };

public:
    explicit C5StoreInventory(C5User *user, const QString &title, QIcon icon, QWidget *parent = nullptr);

    ~C5StoreInventory();

    virtual QToolBar* toolBar();

    void setDocument(StoreInventoryDocument sd);

private slots:

    void saveDoc();

    void printDoc();

    void autofillDoc();

    void exportDiffs();

    void on_btnAddGoods_clicked();

    void on_btnRemoveGoods_clicked();

    void on_leSearch_textChanged(const QString &arg1);

    void on_btnCloseSearch_clicked();

    void on_leFind_textChanged(const QString &arg1);

    void userQtyTextChanged(const QString &arg1);

    void on_btnAddSemiready_clicked();

    void on_btnSemireadyList_clicked();

    void removeDocument();

private:
    Ui::C5StoreInventory* ui;

    QString fInternalID;

    QVector<SemireadyBatch> mSemireadyBatches;

    int addGoodsRow();

    void countTotal();

    bool docCheck(QString &err);

    QString makeInventoryTable(C5TableWidget *tbl);

    int findGoodsRow(int goodsId) const;

    double lockedQty(int row) const;

    void setLockedQty(int row, double qty);

    void refreshRowDiff(int row);

    /** Fills the Stock column from warehouse movements up to the document date. */
    void refreshStockBalances();

    void applySemireadyBatch(const SemireadyBatch &batch);

    void removeSemireadyBatch(const QString &batchId);
};

#endif // C5STOREINVENTORY_H
