#ifndef DLGRESERVGOODS_H
#define DLGRESERVGOODS_H

#include "c5dialog.h"

namespace Ui {
class DlgReservGoods;
}

class DlgReservGoods : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgReservGoods(const QStringList &dbParams, int store, int goods, double qty);

    explicit DlgReservGoods(const QStringList &dbParams, int id);

    explicit DlgReservGoods(const QStringList &dbParams);

    ~DlgReservGoods();

private slots:
    void messageResult(const QJsonObject &jo);

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

    void updateState(const QStringList &dbparams, int state);

    void insertReserve(const QStringList &dbparams);
};

#endif // DLGRESERVGOODS_H
