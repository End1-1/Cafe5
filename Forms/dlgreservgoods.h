#ifndef DLGRESERVGOODS_H
#define DLGRESERVGOODS_H

#include "c5dialog.h"
#include "struct_storage_item.h"
#include "struct_goods_item.h"

namespace Ui
{
class DlgReservGoods;
}

class DlgReservGoods : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgReservGoods(C5User *user, StorageItem store, GoodsItem goods, double qty);

    explicit DlgReservGoods(int id, C5User *user);
    
    explicit DlgReservGoods(C5User *user);

    ~DlgReservGoods();

private slots:

    void createReserveResponse(const QJsonObject &jdoc);

    void on_btnClose_clicked();

    void on_btnSave_clicked();

    void on_btnCancelReserve_clicked();

    void on_btnCompleteReserve_clicked();

    void on_leReservedQty_textEdited(const QString &arg1);

    void on_btnGoods_clicked();

    void on_btnStore_clicked();

    void on_btnPrintFiscal_clicked();

private:
    Ui::DlgReservGoods *ui;

    StorageItem fStore;

    GoodsItem fGoods;

    void getAvailableGoods();

    void setState(int state);

    void updateState(int state);
};

#endif // DLGRESERVGOODS_H
