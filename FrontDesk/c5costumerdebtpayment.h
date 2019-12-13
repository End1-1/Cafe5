#ifndef C5COSTUMERDEBTPAYMENT_H
#define C5COSTUMERDEBTPAYMENT_H

#include "c5dialog.h"

namespace Ui {
class C5CostumerDebtPayment;
}

class C5CostumerDebtPayment : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5CostumerDebtPayment(const QStringList &dbParams);

    ~C5CostumerDebtPayment();

    void setId(int id);

private slots:
    void on_btnCancel_clicked();

    void on_btnOK_clicked();

    void on_btnRemove_clicked();

private:
    Ui::C5CostumerDebtPayment *ui;
};

#endif // C5COSTUMERDEBTPAYMENT_H
