#ifndef DLGRESERVGOODS_H
#define DLGRESERVGOODS_H

#include "c5dialog.h"

namespace Ui
{
class DlgReservGoods;
}

class DlgReservGoods : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgReservGoods(int store, int goods, double qty);

    explicit DlgReservGoods(int id);

    explicit DlgReservGoods();

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

    int fStore;

    int fGoods;

    void getAvailableGoods();

    void setState(int state);

    void updateState(int state);
};

#endif // DLGRESERVGOODS_H
