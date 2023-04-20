#ifndef CR5GOODS_H
#define CR5GOODS_H

#include "c5reportwidget.h"

class CR5GoodsFilter;

class CR5Goods : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5Goods(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

public slots:
    virtual bool on_tblView_doubleClicked(const QModelIndex &index);

private:
    CR5GoodsFilter *fFilter;

private slots:
    void pricing();

    void groupPrice();

    void exportToScales();

    void deleteGoods();

    void printBarCodes();

    void armSoftMap();
};

#endif // CR5GOODS_H
