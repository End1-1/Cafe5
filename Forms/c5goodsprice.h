#ifndef C5GoodsPriceOrder_H
#define C5GoodsPriceOrder_H

#include "c5widget.h"

namespace Ui
{
class C5GoodsPriceOrder;
}

class QListWidgetItem;

class C5GoodsPriceOrder : public C5Widget
{
    Q_OBJECT

public:
    explicit C5GoodsPriceOrder(const QStringList &dbParams);

    ~C5GoodsPriceOrder();

private:
    QDate fDate1, fDate2;

private slots:
    void createGropuDiscountResponse(const QJsonObject &jdoc);

    void getAllResponse(const QJsonObject &jdoc);

    void addGroupResponse(const QJsonObject &jdoc);

    void saveResponse(const QJsonObject &jdoc);

    void discountResponse(const QJsonObject &jdoc);

    void refreshSaleStoreResponse(const QJsonObject &jdoc);

    void tblGoodsContextMenu(const QPoint &p);

    void on_btnAddNew_clicked();

    void on_btnNewGroup_clicked();

    void on_btnSave_clicked();

    void on_lwNames_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_btnShowGoods_clicked();

    void on_tblDoc_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

    void on_btnRollback_clicked();

    void on_tblDoc_customContextMenuRequested(const QPoint &pos);

    void on_actionRemove_triggered();

    void on_btnApply_clicked();

    void on_actionChangeGoodsPrice_triggered();

    void on_actionSet_whosale_price_of_selected_goods_triggered();

    void on_btnChangeRange_clicked();

    void on_btnRefresh_clicked();

private:
    Ui::C5GoodsPriceOrder *ui;

    int newRow();

    int newGoodsRow();

    void setCurrentGroup(int group);

    void getSaledQty();

    void setGroupPriceJson(int row, int col, double price);
};

#endif // C5GoodsPriceOrder_H
