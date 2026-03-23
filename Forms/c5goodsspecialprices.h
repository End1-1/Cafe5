#ifndef C5GOODSSPECIALPRICES_H
#define C5GOODSSPECIALPRICES_H

#include "c5widget.h"

class C5GoodsSpecialPrice;

namespace Ui
{
class C5GoodsSpecialPrices;
}

class QTableWidgetItem;
class C5GoodsSpecialPrice;

class C5GoodsSpecialPrices : public C5Widget
{
    Q_OBJECT

    enum Columns {
        col_id = 0,
        col_partner_id,          // 1
        col_partner_tin,         // 2
        col_partner_name,        // 3
        col_partner_address,     // 4
        col_goods_id,            // 5
        col_goods_group,         // 6
        col_goods_barcode,       // 7
        col_goods_name,          // 8
        col_goods_special_price, // 9
        col_goods_retail,        // 10
        col_goods_whosale,       // 11
        col_goods_retail_diff,   // 12
        col_goods_whosale_diff   // 13
    };

public:
    explicit C5GoodsSpecialPrices(QWidget *parent = nullptr);

    ~C5GoodsSpecialPrices();

    virtual QToolBar *toolBar() override;

private slots:
    void on_tblp_itemClicked(QTableWidgetItem *item);

    void on_btnAdd_clicked();

    void on_btnSave_clicked();

    void on_btnAddGroup_clicked();

    void on_btnDeleteGroup_clicked();

    void on_btnEdit_clicked();

private:
    Ui::C5GoodsSpecialPrices *ui;

    void getPrices();

    void fillRow(C5GoodsSpecialPrice *sp, int row);
};

#endif // C5GOODSSPECIALPRICES_H
