#ifndef DLGSHIFTROTATION_H
#define DLGSHIFTROTATION_H

#include "c5dialog.h"

class C5User;

namespace Ui {
class DlgShiftRotation;
}

class DlgShiftRotation : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgShiftRotation(C5User *user);

    ~DlgShiftRotation();

private slots:

    void on_btnNextDate_clicked();

    void on_btnPrevDate_clicked();

    void on_btnChange_clicked();

    void on_btnCancel_clicked();

private:
    Ui::DlgShiftRotation *ui;

    C5User *fUser;
};

#endif // DLGSHIFTROTATION_H
