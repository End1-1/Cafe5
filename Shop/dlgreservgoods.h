#ifndef DLGRESERVGOODS_H
#define DLGRESERVGOODS_H

#include <QDialog>

namespace Ui {
class DlgReservGoods;
}

class DlgReservGoods : public QDialog
{
    Q_OBJECT

public:
    explicit DlgReservGoods(int store, int goods, double qty, QWidget *parent = nullptr);

    explicit DlgReservGoods(int id, QWidget *parent = nullptr);

    ~DlgReservGoods();

private slots:
    void on_btnClose_clicked();

    void on_btnSave_clicked();

    void on_leReservedQty_textChanged(const QString &arg1);

    void on_btnCancelReserve_clicked();

    void on_btnCompleteReserve_clicked();

    void on_leReservedQty_textEdited(const QString &arg1);

private:
    Ui::DlgReservGoods *ui;

    int fStore;

    int fGoods;

    void setState(int state);
};

#endif // DLGRESERVGOODS_H
