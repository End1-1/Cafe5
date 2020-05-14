#ifndef WORDER_H
#define WORDER_H

#include "goods.h"
#include "c5database.h"
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
    explicit WOrder(int saleType, QWidget *parent = nullptr);

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

    void setDiscount(const QString &label, const QString &value);

    void setPartner(const QString &taxcode, int id, const QString &taxname);

private slots:
    void on_leCash_textChanged(const QString &arg1);

    void on_leCard_textChanged(const QString &arg1);

    void imageLoaded(const QPixmap &img);

    void noImage();

    void on_btnInfo_clicked();

private:
    Ui::WOrder *ui;

    void countTotal();

    QDate fDateOpen;

    QTime fTimeOpen;

    Working *fWorking;

    int fCostumerId;

    int fCardId;

    int fCardMode;

    double fCardValue;

    int fSaleType;

    int fPartner;

    bool returnFalse(const QString &msg, C5Database &db);
};

#endif // WORDER_H
