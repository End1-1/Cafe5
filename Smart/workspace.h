#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "c5dialog.h"
#include "dish.h"

namespace Ui {
class Workspace;
}

class QTableWidgetItem;
class QTableWidget;
class QListWidgetItem;
class C5User;

class Workspace : public C5Dialog
{
    Q_OBJECT

public:
    explicit Workspace(const QStringList &dbParams);

    ~Workspace();

    bool login();

    virtual void reject() override;

private slots:
    void setQty();

    void changeQty();

    void removeDish();

    void on_tblPart2_itemClicked(QTableWidgetItem *item);

    void on_tblDishes_itemClicked(QTableWidgetItem *item);

    void on_btnCheckout_clicked();

    void on_btnClearFilter_clicked();

    void on_btnExit_clicked();

    void on_btnPartUp_clicked();

    void on_btnPartDown_clicked();

    void on_btnDishUp_clicked();

    void on_btnDishDown_clicked();

    void on_btnAny_clicked();

    void on_btnPrintReport_clicked();

    void on_btnPrintReport2_clicked();

    void on_btnCustomer_clicked();

    void on_btnSupplier_clicked();

    void on_btnShowPackages_clicked();

    void on_btnShowDishes_clicked();

    void on_lstCombo_itemClicked(QListWidgetItem *item);

private:
    Ui::Workspace *ui;

    C5User *fUser;

    QList<Dish*> fDishes;

    int fCustomer;

    int fTypeFilter;

    int fSupplierId;

    QString fPhone;

    QString fSupplierName;

    bool currentDish(Dish &d);

    void setCurrentDish(Dish &d);

    void filter();

    void countTotal();

    void printReport(const QDate &d1, const QDate &d2);

    void stretchTableColumns(QTableWidget *t);

    void scrollTable(QTableWidget *t, int direction, int rows);
};

#endif // WORKSPACE_H
