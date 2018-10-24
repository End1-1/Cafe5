#ifndef C5DISHWIDGET_H
#define C5DISHWIDGET_H

#include "c5widget.h"

namespace Ui {
class C5DishWidget;
}

class C5DishWidget : public C5Widget
{
    Q_OBJECT

public:
    explicit C5DishWidget(const QStringList &dbParams, QWidget *parent = 0);
    ~C5DishWidget();

private:
    Ui::C5DishWidget *ui;
};

#endif // C5DISHWIDGET_H
