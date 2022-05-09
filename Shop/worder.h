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
class C5User;

class WOrder : public QWidget
{
    Q_OBJECT

public:
    explicit WOrder(C5User *user, int saleType, QWidget *parent = nullptr);

    ~WOrder();

    int fSaleType;

    QString fOrderUUID;

    void clearCode();

    void keyMinus();

    void keyPlus();

    void keyAsterix();

    void focusCash();

    void focusCard();

    void focusTaxpayerId();

    void imageConfig();

    void addGoods(int id);

    void addGoodsToTable(int id);

    bool writeOrder();

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

    void openPreorder(const QByteArray &id);

    void setDiscount(const QString &label, const QString &value);

    void setPartner(const QString &taxcode, int id, const QString &taxname);

    void setQtyOfRow(int row, double qty);

    C5TableWidget *table();

    bool checkQty(int goods, double qty, QString &err);

private slots:
    void imageLoaded(const QPixmap &img);

    void noImage();

    void on_btnRemove_clicked();

    void on_leCash_textChanged(const QString &arg1);

    void on_leCode_textChanged(const QString &arg1);

    void on_leCode_returnPressed();

    void on_leCustomerTaxpayerId_returnPressed();

    void on_leCard_textChanged(const QString &arg1);

    void on_leAdvance_textChanged(const QString &arg1);

    void on_btnPrintAdvanceTax_clicked();

    void on_btnSavePreorder_clicked();

    void on_lePreorderAdvance_textChanged(const QString &arg1);

    void on_btnRemovePreorder_clicked();

    void on_btnOpenPreorder_clicked();

private:
    Ui::WOrder *ui;

    C5User *fUser;

    QDate fDateOpen;

    QTime fTimeOpen;

    Working *fWorking;

    int fCostumerId;

    int fCardId;

    int fCardMode;

    double fCardValue;

    int fPartner;

    QByteArray fPreorderUUID;

    bool returnFalse(const QString &msg, C5Database &db);

    bool getDiscountValue(int discountType, double &v);

};

#endif // WORDER_H
