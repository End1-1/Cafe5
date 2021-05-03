#ifndef DLGGUESTS_H
#define DLGGUESTS_H

#include "c5dialog.h"

namespace Ui {
class DlgGuests;
}

class C5WaiterOrderDoc;

class DlgGuests : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgGuests(const QStringList &dbParams, C5WaiterOrderDoc *order);

    ~DlgGuests();

private slots:
    void on_btnGuestUp_clicked();

    void on_btnGuestDown_clicked();

    void on_btnSplitOrder_clicked();

private:
    Ui::DlgGuests *ui;

    C5WaiterOrderDoc *fOrder;
};

#endif // DLGGUESTS_H
