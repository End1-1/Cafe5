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

    int fSaleType;

    void changeIshmarColor(const QString &c);

    void focusCash();

    void focusCard();

    void addGoods(const Goods &g);

    void addGoodsToTable(const Goods &g);

    bool writeOrder(bool tax = true);

    bool writePreorder();

    void fixCostumer(const QString &code);

    void changeQty();

    void discountRow(const QString &code);

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

    void on_btnOpenOrder_clicked();

    void on_btnSave_clicked();

    void on_btnPrintTaxPrepaid_clicked();

    void on_btnRemove_clicked();

    void on_leAdvance_textChanged(const QString &arg1);

    void on_btnPrintManualTax_clicked();

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

    int fPartner;

    int fPreorderTax;

    QByteArray fPreorderUUID;

    bool returnFalse(const QString &msg, C5Database &db);

    bool getDiscountValue(int discountType, double &v);
};

#endif // WORDER_H
