#ifndef C5COSTUMERDEBTPAYMENT_H
#define C5COSTUMERDEBTPAYMENT_H

#include "c5dialog.h"
#include "bclientdebts.h"
#include "aheader.h"
#include "ecash.h"

namespace Ui
{
class C5CostumerDebtPayment;
}

class C5CostumerDebtPayment : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5CostumerDebtPayment(C5User *user, int type);

    ~C5CostumerDebtPayment();

    void setId(const QString &id);

    void setPartnerAndAmount(int partner, double amount, const QString &clearFlag);

    virtual void selectorCallback(int row, const QJsonArray &values) override;

private slots:
    void on_btnCancel_clicked();

    void on_btnOK_clicked();

    void on_btnRemove_clicked();

private:
    Ui::C5CostumerDebtPayment* ui;

    BClientDebts fBClientDebt;

    AHeader fAHeader;

    ECash fECash;

    QString fClearFlag;
};

#endif // C5COSTUMERDEBTPAYMENT_H
