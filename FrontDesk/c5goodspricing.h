#ifndef C5GOODSPRICING_H
#define C5GOODSPRICING_H

#include "c5widget.h"

namespace Ui {
class C5GoodsPricing;
}

class C5GoodsPricing : public C5Widget
{
    Q_OBJECT

public:
    explicit C5GoodsPricing(const QStringList &dbParams, QWidget *parent = nullptr);

    ~C5GoodsPricing();

private:
    Ui::C5GoodsPricing *ui;
};

#endif // C5GOODSPRICING_H
