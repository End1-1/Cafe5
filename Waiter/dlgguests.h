#ifndef DLGGUESTS_H
#define DLGGUESTS_H

#include "c5dialog.h"

namespace Ui
{
class DlgGuests;
}

class DlgGuests : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgGuests(C5User *user);

    ~DlgGuests();

    void setGuests(int value);

    int guests();

private slots:
    void on_btnGuestUp_clicked();

    void on_btnGuestDown_clicked();

    void on_btnAccept_clicked();

    void on_btnCancel_clicked();

private:
    Ui::DlgGuests* ui;
};

#endif // DLGGUESTS_H
