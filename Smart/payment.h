#ifndef PAYMENT_H
#define PAYMENT_H

#include "c5dialog.h"

namespace Ui {
class payment;
}

class QTableWidget;

class payment : public C5Dialog
{
    Q_OBJECT

public:
    explicit payment(const QStringList &dbParams);

    ~payment();

    void setAmount(double amount);

    double cardAmount();

    double cashAmount();

    void setOrderTable(QTableWidget *t);

    QString fOrderUUID;

private slots:
    void on_btnTax_clicked(bool checked);

    void on_btnCancel_clicked();

    void on_leCash_textChanged(const QString &arg1);

    void on_btnCheckoutCash_clicked();

    void on_btnCheckoutCard_clicked();

private:
    Ui::payment *ui;

    QTableWidget *fOrderTable;

    double fCardAmount;

    void checkout(bool cash);

    bool printReceipt();

    bool printTax(double cardAmount);
};

#endif // PAYMENT_H
