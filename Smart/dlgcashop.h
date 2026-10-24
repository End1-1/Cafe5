#ifndef DLGCASHOP_H
#define DLGCASHOP_H

#include "c5dialog.h"

namespace Ui {
class DlgCashOp;
}

class C5User;

class DlgCashOp : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgCashOp(bool in, C5User *u);
    ~DlgCashOp();

private slots:
    void on_btnCancel_clicked();

    void on_btnSave_clicked();

private:
    Ui::DlgCashOp *ui;

    C5User *fUser;

    bool fSign;
};

#endif // DLGCASHOP_H
