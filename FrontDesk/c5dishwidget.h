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

    void setDish(int id);

    void clearWidget();

    void save(int id);

private slots:
    void on_btnAddRecipe_clicked();

    void on_btnRemoveRecipe_clicked();

private:
    Ui::C5DishWidget *ui;

    int fId;

    void countTotalSelfCost();
};

#endif // C5DISHWIDGET_H
