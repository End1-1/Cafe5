#ifndef C5GOODSSPECIALPRICE_H
#define C5GOODSSPECIALPRICE_H

#include "c5dialog.h"

namespace Ui
{
class C5GoodsSpecialPrice;
}

class C5GoodsSpecialPrice : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5GoodsSpecialPrice(QWidget *parent = nullptr);

    ~C5GoodsSpecialPrice();

private slots:
    void on_leSpecialPrice_textChanged(const QString &arg1);

    void on_btnOK_clicked();

    void on_btnCancel_clicked();

    void on_btnSelectPartner_clicked();

    void on_btnSelectGoods_clicked();

private:
    Ui::C5GoodsSpecialPrice *ui;
};

#endif // C5GOODSSPECIALPRICE_H
