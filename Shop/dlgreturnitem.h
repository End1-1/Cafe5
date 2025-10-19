#ifndef DLGRETURNITEM_H
#define DLGRETURNITEM_H

#include "c5dialog.h"

namespace Ui
{
class DlgReturnItem;
}

class DlgReturnItem : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgReturnItem();

    ~DlgReturnItem();

    void setMode(int mode);

private slots:
    void checkQtyResponse(const QJsonObject &jdoc);

    void on_btnSearchReceiptNumber_clicked();

    void on_tblOrder_cellClicked(int row, int column);

    void on_btnSearchTax_clicked();

    void on_btnReturn_clicked();

    void on_leExchange_returnPressed();

    void on_btnClose_clicked();

private:
    Ui::DlgReturnItem* ui;

    int fMode;

    void countReturnAmount();
};

#endif // DLGRETURNITEM_H
