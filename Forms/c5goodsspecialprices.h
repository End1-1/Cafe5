#ifndef C5GOODSSPECIALPRICES_H
#define C5GOODSSPECIALPRICES_H

#include "c5widget.h"

namespace Ui {
class C5GoodsSpecialPrices;
}

class C5GoodsSpecialPrices : public C5Widget
{
    Q_OBJECT

public:
    explicit C5GoodsSpecialPrices(const QStringList &dbParams, QWidget *parent = nullptr);
    ~C5GoodsSpecialPrices();
    virtual QToolBar *toolBar() override;

private slots:
    void on_cbPartner_currentIndexChanged(int index);
    void calcPercent(const QString &arg1);
    void saveDataChanges();

private:
    Ui::C5GoodsSpecialPrices *ui;
    int fPartner;
    void getPrices();
};

#endif // C5GOODSSPECIALPRICES_H
