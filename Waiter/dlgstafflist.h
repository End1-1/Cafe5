#ifndef DLGSTAFFLIST_H
#define DLGSTAFFLIST_H

#include "c5dialog.h"

namespace Ui {
class DlgStaffList;
}

class DlgStaffList : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgStaffList();

    ~DlgStaffList();

private:
    Ui::DlgStaffList *ui;

private slots:
    void searchStaff(const QString &txt);
    void on_tbl_cellClicked(int row, int column);
};

#endif // DLGSTAFFLIST_H
