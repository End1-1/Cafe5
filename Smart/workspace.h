#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "c5dialog.h"
#include "c5user.h"
#include "dish.h"

namespace Ui {
class Workspace;
}

class QTableWidgetItem;
class QTableWidget;

class Workspace : public C5Dialog
{
    Q_OBJECT

public:
    explicit Workspace(const QStringList &dbParams);

    ~Workspace();

    void login();

private slots:
    void setQty();

    void changeQty();

    void removeDish();

    void on_tblPart2_itemClicked(QTableWidgetItem *item);

    void on_tblDishes_itemClicked(QTableWidgetItem *item);

    void on_btnCheckout_clicked();

    void on_btnClearFilter_clicked();

private:
    Ui::Workspace *ui;

    C5User fUser;

    QList<Dish*> fDishes;

    int fTypeFilter;

    bool currentDish(Dish &d);

    void setCurrentDish(Dish &d);

    void filter();

    void countTotal();

    void stretchTableColumns(QTableWidget *t);
};

#endif // WORKSPACE_H
