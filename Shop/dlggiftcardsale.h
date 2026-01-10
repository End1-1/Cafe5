#ifndef DLGGIFTCARDSALE_H
#define DLGGIFTCARDSALE_H

#include "c5dialog.h"

namespace Ui
{
class DlgGiftCardSale;
}

class DlgGiftCardSale : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgGiftCardSale(C5User *user);

    ~DlgGiftCardSale();

    int fGiftGoodsId;

    QString fGiftScancode;

    double fGiftPrice;

private slots:
    void on_leCode_returnPressed();

    void on_btnRegister_clicked();

    void on_btnCancel_clicked();

private:
    Ui::DlgGiftCardSale* ui;
};

#endif // DLGGIFTCARDSALE_H
