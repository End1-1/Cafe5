#ifndef C5GOODSSPECIALPRICES_H
#define C5GOODSSPECIALPRICES_H

#include "c5widget.h"

namespace Ui {
class C5GoodsSpecialPrices;
}

class QTableWidgetItem;

class C5GoodsSpecialPrices : public C5Widget
{
    Q_OBJECT

public:
    explicit C5GoodsSpecialPrices(const QStringList &dbParams, QWidget *parent = nullptr);
    ~C5GoodsSpecialPrices();
    virtual QToolBar *toolBar() override;

private slots:
    void calcPercent1(const QString &arg1);
    void calcPercent2(const QString &arg1);
    void saveDataChanges();
    void on_tblp_itemActivated(QTableWidgetItem *item);

    void on_tblp_itemClicked(QTableWidgetItem *item);

    void on_lePartnerFilter_textChanged(const QString &arg1);

    void on_leGoodsFilter_textChanged(const QString &arg1);

    void on_leGroupFilter_textChanged(const QString &arg1);

private:
    Ui::C5GoodsSpecialPrices *ui;
    int fPartner;
    void getPrices();
    void filterGoods();
};

#endif // C5GOODSSPECIALPRICES_H

