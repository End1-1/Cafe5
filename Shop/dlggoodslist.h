#ifndef DLGGOODSLIST_H
#define DLGGOODSLIST_H

#include "c5dialog.h"

namespace Ui {
class DlgGoodsList;
}

class DlgGoodsList : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgGoodsList();
    ~DlgGoodsList();

    static bool getGoods(int &id);

private slots:
    void on_leSearch_textChanged(const QString &arg1);

private:
    Ui::DlgGoodsList *ui;
};

#endif // DLGGOODSLIST_H
