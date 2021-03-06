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

    virtual void selectorCallback(int row, const QList<QVariant> &values) override;

private slots:
    void selectorDone(const QList<QVariant> &values);

    void on_btnCancel_clicked();

    void on_btnOK_clicked();

    void on_btnRemove_clicked();

private:
    Ui::C5CostumerDebtPayment *ui;

    QString fSelectedGovNumber;
};

#endif // C5COSTUMERDEBTPAYMENT_H
