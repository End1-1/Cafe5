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

public:
    explicit C5GoodsSpecialPrices(const QStringList &dbParams, QWidget *parent = nullptr);

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
