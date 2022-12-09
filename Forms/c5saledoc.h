#ifndef C5SALEDOC_H
#define C5SALEDOC_H

#include "c5widget.h"

namespace Ui {
class C5SaleDoc;
}

static const int PRICEMODE_RETAIL = 1;
static const int PRICEMODE_WHOSALE = 2;

class C5SaleDoc : public C5Widget
{
    Q_OBJECT

public:
    explicit C5SaleDoc(const QStringList &dbParams, QWidget *parent = nullptr);
    ~C5SaleDoc();
    void setMode(int mode);

private slots:
    void on_PriceTextChanged(const QString &arg1);
    void on_QtyTextChanged(const QString &arg1);
    void on_TotalTextChanged(const QString &arg1);
    void on_leCmd_returnPressed();

private:
    Ui::C5SaleDoc *ui;
    int fMode;
    void addGoods(int id, C5Database &db);
    void countGrandTotal();
};

#endif // C5SALEDOC_H
