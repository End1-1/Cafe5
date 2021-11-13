#ifndef WORDER_H
#define WORDER_H

#include "c5database.h"
#include <QWidget>
#include <QDate>
#include <QTime>

namespace Ui {
class WOrder;
}

class Working;
class C5TableWidget;

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

    void addGoods(int id);

    void addGoodsToTable(int id);

    bool writeOrder(bool tax = true);

    bool writePreorder();

    void fixCostumer(const QString &code);

    void changeQty();

    void discountRow(const QString &code);

    void changePrice();

    int rowCount();

    void removeRow();

    void nextRow();

    void prevRow();

    int lastRow();

    void countTotal();

    void setDiscount(const QString &label, const QString &value);

    void setPartner(const QString &taxcode, int id, const QString &taxname);

    void setQtyOfRow(int row, double qty);

    C5TableWidget *table();

    bool checkQty(int goods, double qty, QString &err);

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
