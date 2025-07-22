#ifndef DLGGUESTS_H
#define DLGGUESTS_H

#include "c5dialog.h"

namespace Ui
{
class DlgGuests;
}

class C5OrderDriver;

class DlgGuests : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgGuests(C5OrderDriver *order);

    ~DlgGuests();

private slots:
    void on_btnGuestUp_clicked();

    void on_btnGuestDown_clicked();

    void on_btnSplitOrder_clicked();

    void on_btnClose_clicked();

private:
    Ui::DlgGuests* ui;

    C5OrderDriver* fOrder;
};

#endif // DLGGUESTS_H
