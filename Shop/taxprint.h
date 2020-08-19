#ifndef TAXPRINT_H
#define TAXPRINT_H

#include "c5dialog.h"

namespace Ui {
class TaxPrint;
}

class TaxPrint : public C5Dialog
{
    Q_OBJECT

public:
    explicit TaxPrint();

    ~TaxPrint();

private slots:
    void on_btnPlus_clicked();

    void qtyChanged(const QString &arg1);

    void priceChanged(const QString &arg1);

    void totalChanged(const QString &arg1);

    void on_leCode_returnPressed();

    void on_btnPrint_clicked();

    void on_btnClear_clicked();

private:
    Ui::TaxPrint *ui;

    void addGoods(const QString &code);

    void countTotal();
};

#endif // TAXPRINT_H
