#ifndef DLGGOODSLIST_H
#define DLGGOODSLIST_H

#include "c5shopdialog.h"

namespace Ui
{
class DlgGoodsList;
}

class DlgGoodsList : public C5ShopDialog
{
    Q_OBJECT

public:
    explicit DlgGoodsList(C5User *user, int currency);

    ~DlgGoodsList();

protected:
    virtual bool event(QEvent *event) override;

private slots:
    void on_leSearch_textChanged(const QString &arg1);

    void on_btnExit_clicked();

    void on_btnMinimize_clicked();

private:
    Ui::DlgGoodsList* ui;

    int fGoodsId;

signals:
    void getGoods(int, double, double, double);
};

#endif // DLGGOODSLIST_H
