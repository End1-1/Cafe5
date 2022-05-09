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

protected:
    virtual bool event(QEvent *event) override;

private slots:
    void on_leSearch_textChanged(const QString &arg1);

    void on_btnExit_clicked();

    void on_btnMinimize_clicked();

private:
    Ui::DlgGoodsList *ui;

    int fGoodsId;
};

#endif // DLGGOODSLIST_H
