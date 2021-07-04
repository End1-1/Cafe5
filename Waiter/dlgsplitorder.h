#ifndef DLGSPLITORDER_H
#define DLGSPLITORDER_H

#include "c5dialog.h"
#include "c5user.h"

namespace Ui {
class DlgSplitOrder;
}

class C5OrderDriver;

class DlgSplitOrder : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgSplitOrder(C5User *user);

    ~DlgSplitOrder();

    void configOrder(const QString &orderId);

private slots:
    void on_btnChoseTable_clicked();

    void on_btnExit_clicked();

    void on_btnLeftToRight_clicked();

    void on_btnAllLeftToRigh_clicked();

    void on_btnRightToLeft_clicked();

    void on_btnAllRightToLeft_clicked();

    void on_btnSave_clicked();

private:
    Ui::DlgSplitOrder *ui;

    C5User *fUser;

    bool moveItem(C5OrderDriver *or1, C5OrderDriver *or2, int row);
};

#endif // DLGSPLITORDER_H
