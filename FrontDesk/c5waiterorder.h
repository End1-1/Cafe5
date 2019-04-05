#ifndef C5WAITERORDER_H
#define C5WAITERORDER_H

#include "c5widget.h"

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

private:
    Ui::C5WaiterOrder *ui;
};

#endif // C5WAITERORDER_H
