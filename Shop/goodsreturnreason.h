#ifndef GOODSRETURNREASON_H
#define GOODSRETURNREASON_H

#include "c5dialog.h"

namespace Ui {
class GoodsReturnReason;
}

class QListWidgetItem;

class GoodsReturnReason : public C5Dialog
{
    Q_OBJECT

public:
    explicit GoodsReturnReason();

    ~GoodsReturnReason();

    int fReason;

private slots:
    void on_lst_itemClicked(QListWidgetItem *item);

private:
    Ui::GoodsReturnReason *ui;
};

#endif // GOODSRETURNREASON_H
