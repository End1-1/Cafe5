#ifndef C5GOODSPRICING_H
#define C5GOODSPRICING_H

#include "c5dialog.h"

namespace Ui {
class C5GoodsPricing;
}

class C5GoodsPricing : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5GoodsPricing();

    ~C5GoodsPricing();

    int roundValue();

private slots:
    void on_btnCancel_clicked();

    void on_btnOK_clicked();

private:
    Ui::C5GoodsPricing *ui;
};

#endif // C5GOODSPRICING_H
