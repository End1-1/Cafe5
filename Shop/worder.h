#ifndef WORDER_H
#define WORDER_H

#include "goods.h"
#include <QWidget>
#include <QDate>
#include <QTime>

namespace Ui {
class WOrder;
}

class Working;

class WOrder : public QWidget
{
    Q_OBJECT

public:
    explicit WOrder(QWidget *parent = nullptr);

    ~WOrder();

    void focusCash();

    void focusCard();

    void addGoods(const Goods &g);

    bool writeOrder(bool tax = true);

    void fixCostumer(const QString &code);

    void changeQty();

    void changePrice();

    void removeRow();

    void nextRow();

    void prevRow();

    void refund();

    void setDiscount(const QString &label, const QString &value);

private slots:
    void on_leCash_textChanged(const QString &arg1);

    void on_leCard_textChanged(const QString &arg1);

private:
    Ui::WOrder *ui;

    void countTotal();

    QDate fDateOpen;

    QTime fTimeOpen;

    Working *fWorking;

    bool fModeRefund;

    int fCostumerId;

    int fCardId;

    int fCardMode;

    double fCardValue;

    double fCardData;
};

#endif // WORDER_H
