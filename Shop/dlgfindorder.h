#ifndef DLGFINDORDER_H
#define DLGFINDORDER_H

#include "c5shopdialog.h"

namespace Ui
{
class DlgFindOrder;
}

class DlgFindOrder : public C5ShopDialog
{
    Q_OBJECT

public:
    explicit DlgFindOrder(C5User *user, QWidget *parent = nullptr);

    ~DlgFindOrder() override;

    QString orderId() const { return mOrderId; }

private slots:
    void on_btnOk_clicked();

    void on_btnCancel_clicked();

    void on_lePrefix_returnPressed();

private:
    void findOrder();

    Ui::DlgFindOrder *ui = nullptr;

    QString mOrderId;
};

#endif // DLGFINDORDER_H
