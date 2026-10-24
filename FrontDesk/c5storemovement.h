#pragma once

#include "c5widget.h"
#include "semiready_batch.h"
#include "struct_doc_store_input.h"
#include <QVector>

namespace Ui
{
class C5StoreMovement;
}

class C5StoreMovement : public C5Widget
{
    Q_OBJECT

public:
    explicit C5StoreMovement(C5User *user, const QString &title, QIcon icon, QWidget *parent = nullptr);
    ~C5StoreMovement() override;

    void setDocument(StoreInputDocument doc);
    virtual QToolBar *toolBar() override;

private:
    enum Columns {
        col_goods_id = 0,
        col_goods_name,
        col_goods_qty,
        col_goods_unit,
        col_price,
        col_total
    };

    Ui::C5StoreMovement *ui;
    StoreInputDocument mDocData;
    QVector<SemireadyBatch> mSemireadyBatches;
    QAction *mActionSave = nullptr;
    QAction *mActionDraft = nullptr;

    bool buildDoc();
    void setState();
    void countTotal();
    int addGoodsRow();
    int addGoods(int goods, const QString &name, double qty, const QString &unit, double price);

    int findGoodsRow(int goodsId) const;
    double lockedQty(int row) const;
    void setLockedQty(int row, double qty);
    void applySemireadyBatch(const SemireadyBatch &batch);
    void removeSemireadyBatch(const QString &batchId);
    void refreshRowTotal(int row);

private slots:
    void saveDocument();
    void draftDocument();
    void removeDocument();
    void tblQtyChanged(const QString &arg1);
    void tblPriceChanged(const QString &arg1);
    void on_btnAddGoods_clicked();
    void on_btnRemoveGoods_clicked();
    void on_btnAddSemiready_clicked();
    void on_btnSemireadyList_clicked();
};
