#ifndef C5WAITERORDER_H
#define C5WAITERORDER_H

#include "c5widget.h"
#include "c5waiterorderdoc.h"

namespace Ui {
class C5WaiterOrder;
}

class C5WaiterOrder : public C5Widget
{
    Q_OBJECT

public:
    explicit C5WaiterOrder(const QStringList &dbParams, QWidget *parent = nullptr);

    ~C5WaiterOrder();

    void setOrder(const QString &id);

    virtual bool allowChangeDatabase();

    virtual QToolBar *toolBar();

private:
    Ui::C5WaiterOrder *ui;

    void jsonToDoc(C5WaiterOrderDoc &doc);

private slots:
    void transferToHotel();

    void recountSelfCost();

    void openMenuItem();

    void on_tblDishes_customContextMenuRequested(const QPoint &pos);
};

#endif // C5WAITERORDER_H
