#ifndef C5WAITERORDER_H
#define C5WAITERORDER_H

#include "c5officewidget.h"

namespace Ui
{
class C5WaiterOrder;
}

class C5WaiterOrder : public C5OfficeWidget
{
    Q_OBJECT

public:
    explicit C5WaiterOrder(QWidget *parent = nullptr);

    ~C5WaiterOrder();

    void setOrder(const QString &id);

    virtual bool allowChangeDatabase();

    virtual QToolBar* toolBar();

private:
    Ui::C5WaiterOrder* ui;

    void showLog();

    void showStore();

private slots:
    void saveOrder();

    void removeOrder();

    void showAll();

    void hideRemoved();

    void transferToHotel();

    void recountSelfCost();

    void storeOutput();

    void openMenuItem();

    void on_tblDishes_customContextMenuRequested(const QPoint &pos);

    void on_tabWidget_currentChanged(int index);

    void on_btnCopyUUID_clicked();

    void on_btnClearTax_clicked();

    void on_btnSetCL_clicked();

    void on_btnClearCL_clicked();

    void on_rbDraft_clicked(bool checked);

    void on_rbWrited_clicked(bool checked);
};

#endif // C5WAITERORDER_H
