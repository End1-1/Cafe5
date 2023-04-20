#ifndef GOODSASMAP_H
#define GOODSASMAP_H

#include "c5widget.h"

namespace Ui {
class GoodsAsMap;
}

class GoodsAsMap : public C5Widget
{
    Q_OBJECT

public:
    explicit GoodsAsMap(const QStringList &dbParams, QWidget *parent = nullptr);
    ~GoodsAsMap();
    virtual QToolBar *toolBar() override;

private:
    Ui::GoodsAsMap *ui;
    QMap<int, int> asList;
    QMap<int, int> asColId;

private slots:
    virtual void saveDataChanges();
};

#endif // GOODSASMAP_H
