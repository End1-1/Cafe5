#ifndef CR5GOODS_H
#define CR5GOODS_H

#include "c5reportwidget.h"
#include <QTimer>

class CR5GoodsFilter;

class CR5Goods : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5Goods(QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

public slots:
    virtual bool on_tblView_doubleClicked(const QModelIndex &index);

private:
    CR5GoodsFilter *fFilter;

    QTimer mTimer;

private slots:
    void pricing();

    void groupPrice();

    void exportToScales();

    void deleteGoods();

    void printBarCodes();

    void armSoftMap();

    void buildWeb();

    void buildWebResponse(const QJsonObject &obj);

    void semiReadyPriceUpdate();

    void semiReadyPriceUpdateResponse(const QJsonObject jdoc);

signals:
    void messageReceived();
};

#endif // CR5GOODS_H
