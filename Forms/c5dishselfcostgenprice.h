#ifndef C5DISHSELFCOSTGENPRICE_H
#define C5DISHSELFCOSTGENPRICE_H

#include "c5widget.h"

namespace Ui {
class C5DishSelfCostGenPrice;
}

class C5DishSelfCostGenPrice : public C5Widget
{
    Q_OBJECT

public:
    explicit C5DishSelfCostGenPrice(QWidget *parent = nullptr);

    ~C5DishSelfCostGenPrice();

    virtual QToolBar *toolBar();

private slots:
    void on_btnStart_clicked();

    void saveDataChanges();

    void on_leSearch_textChanged(const QString &arg1);

private:
    Ui::C5DishSelfCostGenPrice *ui;

    QMap<int, int> fGoodsRowMap;
};

#endif // C5DISHSELFCOSTGENPRICE_H
