#pragma once

#include "c5dialog.h"
#include <QJsonObject>

namespace Ui
{
class C5RevenueCashOp;
}

class C5RevenueCashOp : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5RevenueCashOp(C5User *user, QWidget *parent = nullptr);
    ~C5RevenueCashOp() override;

    void setCashboxAndCurrency(int cashboxId, int currencyId);

    void applyOperationForEdit(const QJsonObject &operation);

private slots:
    void on_btnSave_clicked();
    void on_btnCancel_clicked();
    void on_btnCashbox_clicked();

private:
    Ui::C5RevenueCashOp *ui;
    int mCashboxId = 0;
    int mCurrencyId = 1;
    int mOperationId = 0;
};

