#ifndef DLGCASHINOUT_H
#define DLGCASHINOUT_H

#include "c5dialog.h"

namespace Ui
{
class DlgCashinOut;
}

class DlgCashinOut : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgCashinOut(C5User *u);

    static int cashinout(C5User *u);

    ~DlgCashinOut();

private slots:
    void on_btnCancel_clicked();

    void on_btnCloseSession_clicked();

private:
    Ui::DlgCashinOut* ui;

    double fBalance;
};

#endif // DLGCASHINOUT_H
