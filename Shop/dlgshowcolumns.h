#ifndef DLGSHOWCOLUMNS_H
#define DLGSHOWCOLUMNS_H

#include "c5dialog.h"

namespace Ui {
class DlgShowColumns;
}

class DlgShowColumns : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgShowColumns();
    ~DlgShowColumns();

private slots:
    void on_chGroup_clicked(bool checked);

    void on_chUnit_clicked(bool checked);

    void on_chBox_clicked(bool checked);

    void on_btnClose_clicked();

private:
    Ui::DlgShowColumns *ui;
};

#endif // DLGSHOWCOLUMNS_H
