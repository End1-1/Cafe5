#pragma once

#include "struct_waiter_dish.h"
#include "waiterorderitemwidget.h"

namespace Ui
{
class WaiterDishWidget;
}

class WaiterDishWidget : public WaiterOrderItemWidget
{
    Q_OBJECT

public:
    explicit WaiterDishWidget(const WaiterDish &d, bool showRemoved, QWidget *parent = nullptr);

    ~WaiterDishWidget();

    virtual void updateDish(WaiterDish value) override;

    void setCheckMode(bool v);

    void setChecked(bool v);

    bool isChecked();

    void pulish();

    bool mShowRemoved;

protected:
    virtual bool event(QEvent *event) override;

    virtual QWidget* container() override;

private slots:
    void on_btnDish_clicked();

private:
    Ui::WaiterDishWidget* ui;

};
