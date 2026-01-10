#ifndef DLGGUESTINFO_H
#define DLGGUESTINFO_H

#include "c5dialog.h"

namespace Ui {
class DlgGuestInfo;
}

class DlgGuestInfo : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgGuestInfo(C5User *user);
    ~DlgGuestInfo();
    int fCode;

private slots:
    void on_btnCancel_clicked();
    void on_btnSave_clicked();

private:
    Ui::DlgGuestInfo *ui;

};

#endif // DLGGUESTINFO_H
