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
    explicit DlgReservGoods(int store, int goods, double qty);

    explicit DlgReservGoods(int id);

    ~DlgReservGoods();

private slots:
    void on_btnClose_clicked();

    void on_btnSave_clicked();

    void on_btnCancelReserve_clicked();

    void on_btnCompleteReserve_clicked();

    void on_leReservedQty_textEdited(const QString &arg1);

private:
    Ui::DlgReservGoods *ui;

    int fStore;

    int fGoods;

    void setState(int state);

    void updateState(const QStringList &dbparams, int state);

    void insertReserve(const QStringList &dbparams);
};

#endif // DLGRESERVGOODS_H
