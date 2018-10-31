#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "c5dialog.h"
#include "c5user.h"

namespace Ui {
class Workspace;
}

class QTableWidgetItem;
class QTableWidget;

struct Dish {
    int id;
    int typeId;
    QString name;
    QString printer;
    QString adgCode;
    int store;
    double price;
    double qty;
    int color;
    Dish() {
        id = 0;
        typeId = 0;
        store = 0;
        price = 0;
        qty = 1;
        color = -1;
    }
};
Q_DECLARE_METATYPE(Dish*)
Q_DECLARE_METATYPE(Dish)

class Workspace : public C5Dialog
{
    Q_OBJECT

public:
    explicit Workspace(const QStringList &dbParams, QWidget *parent = 0);

    ~Workspace();

    void login();

private slots:

    void on_tblPart2_itemClicked(QTableWidgetItem *item);

    void on_tblDishes_itemClicked(QTableWidgetItem *item);

    void on_btnCheckout_clicked();

    void on_btnClearFilter_clicked();

    void on_btnVoid_clicked();

private:
    Ui::Workspace *ui;

    C5User fUser;

    QList<Dish*> fDishes;

    int fTypeFilter;

    void filter();

    void countTotal();

    void stretchTableColumns(QTableWidget *t);
};

#endif // WORKSPACE_H
