#ifndef DLGSPLITORDER_H
#define DLGSPLITORDER_H

#include "c5waiterdialog.h"
#include "struct_waiter_order.h"

namespace Ui
{
class DlgSplitOrder;
}

class C5OrderDriver;
class QVBoxLayout;

class DlgSplitOrder : public C5WaiterDialog
{
    Q_OBJECT

public:
    explicit DlgSplitOrder(WaiterOrder wo, C5User *user);

    ~DlgSplitOrder();

    virtual void accept() override;

    virtual void reject() override;

private slots:

    void focused1(const QString &id);

    void focused2(const QString &id);

    void on_btnChoseTable_clicked();

    void on_btnExit_clicked();

    void on_btnLeftToRight_clicked();

    void on_btnAllLeftToRigh_clicked();

    void on_btnRightToLeft_clicked();

    void on_btnAllRightToLeft_clicked();

    void on_btnSave_clicked();

private:
    Ui::DlgSplitOrder* ui;

    WaiterOrder mOrder;

    WaiterOrder mOrder1;

    WaiterOrder mOrder2;

    void restoreLeftSide();

    bool moveItem(C5OrderDriver *or1, C5OrderDriver *or2, int row);

    QMultiMap<QString, QString> fHistory;

    void overrideAcceptReject(bool isAccept);

    void moveItemLeftRight(int index, WaiterOrder &w1, WaiterOrder &w2, QVBoxLayout *l1, QVBoxLayout *l2);
};

#endif // DLGSPLITORDER_H
