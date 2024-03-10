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

    void on_chDiscount_clicked(bool checked);

    void on_chEmarks_clicked(bool checked);

    void on_chCheckDiscount_clicked(bool checked);

private:
    Ui::DlgShowColumns *ui;
};

#endif // DLGSHOWCOLUMNS_H
