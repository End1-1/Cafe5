#pragma once

#include <QWidget>
#include "struct_waiter_dish.h"

class WaiterOrderItemWidget : public QWidget
{

    Q_OBJECT

public:
    WaiterOrderItemWidget(const WaiterDish &wd, QWidget *parent = nullptr);

    WaiterDish mOrderItem;

    void setFocused(bool value);

    bool isFocused();

    virtual void updateDish(WaiterDish value);

public slots:
    void checkFocus(const QString &id);

protected:
    virtual QWidget* container() = 0;

private:
    bool fFocus = false;

signals:
    void focused(QString);
};
