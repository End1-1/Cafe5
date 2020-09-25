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
    explicit payment(const QString order, const QStringList &dbParams);

    ~payment();

private slots:
    void on_btnTax_clicked(bool checked);

    void on_btnCancel_clicked();

    void on_leCash_textChanged(const QString &arg1);

    void on_btnCheckoutCash_clicked();

    void on_btnCheckoutCard_clicked();

    void on_tblChange_cellClicked(int row, int column);

private:
    Ui::payment *ui;

    QString fOrderUUID;

    void checkout(bool cash);

    bool printReceipt();

    bool printTax(double cardAmount);
};

#endif // PAYMENT_H
