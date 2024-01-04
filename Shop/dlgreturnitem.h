#ifndef DLGRETURNITEM_H
#define DLGRETURNITEM_H

#include "c5dialog.h"

namespace Ui {
class DlgReturnItem;
}

class DlgReturnItem : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgReturnItem();
    ~DlgReturnItem();

private slots:
    void on_btnSearchReceiptNumber_clicked();

    void on_tblOrder_cellClicked(int row, int column);

    void on_btnSearchTax_clicked();

    void on_btnReturn_clicked();

    void on_btnReturnCash_clicked();

    void on_btnReturnCard_clicked();

private:
    Ui::DlgReturnItem *ui;
};

#endif // DLGRETURNITEM_H
