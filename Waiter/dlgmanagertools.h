#ifndef DLGMANAGERTOOLS_H
#define DLGMANAGERTOOLS_H

#include "c5dialog.h"

class C5User;

namespace Ui {
class DlgManagerTools;
}

class DlgManagerTools : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgManagerTools(C5User *u);

    ~DlgManagerTools();

private slots:
    void on_pushButton_clicked();

    void on_btnChangeSession_clicked();

    void on_btnChangeMyPassword_clicked();

    void on_btnChangeStaffPassword_clicked();

    void on_btnExit_clicked();

private:
    Ui::DlgManagerTools *ui;

    C5User *fUser;
};

#endif // DLGMANAGERTOOLS_H
