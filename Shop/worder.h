#ifndef WORDER_H
#define WORDER_H

#include "c5database.h"
#include "c5dbrecord.h"
#include "bhistory.h"
#include "oheader.h"
#include "ogoods.h"
#include "odraftsale.h"
#include "jsons.h"
#include <QWidget>
#include <QDate>
#include <QTime>

#define col_bacode 0
#define col_group 1
#define col_name 2
#define col_qty 3
#define col_qtybox 4
#define col_unit 5
#define col_price 6
#define col_total 7
#define col_discamount 8
#define col_discmode 9
#define col_discvalue 10
#define col_stock 11
#define col_qr 12
#define col_check_discount 13

namespace Ui {
class WOrder;
}

class Working;
class C5ClearTableWidget;
class C5User;
class WCustomerDisplay;

class WOrder : public QWidget
{
    Q_OBJECT

public:
    explicit WOrder(C5User *user, int saleType, WCustomerDisplay *customerDisplay, QWidget *parent = nullptr);

    ~WOrder();

    ODraftSale fDraftSale;

    OHeader fOHeader;

    void setPrepaidAmount(double amountPrepaid);

    void updateCustomerDisplay(WCustomerDisplay *cd);

    void clearCode();

    void keyMinus();

    void keyPlus();

    void keyAsterix();

    void focusTaxpayerId();

    void imageConfig();

    bool addGoods(int id, double storeqty, double price1, double price2);

    int addGoodsToTable(int id, bool checkQtyOfStore, double qtyStore, const QString &draftid, double price1, double price2);

    bool writeOrder();

    void fixCostumer(const QString &code);

    void changeQty();

    void changeQty(double qty);

    void discountRow(const QString &code);

    void changePrice();

    void changePrice(double price);

    int rowCount();

    void removeRow();

    void nextRow();

    void prevRow();

    int lastRow();

    void comma();

    void countTotal();

    void setDiscount(const QString &label, const QString &value);

    bool setQtyOfRow(int row, double qty);

    bool setPriceOfRow(int row, double price);

    C5ClearTableWidget *table();

    bool checkQty(int goods, double qty, double oldqty, bool updateStock, QString &err);

private slots:
    void imageLoaded(const QPixmap &img);

    void readEmarks();

    void noImage();

    void checkCardClicked(bool v);

    void on_leCode_textChanged(const QString &arg1);

    void on_leCode_returnPressed();

    void on_leCustomerTaxpayerId_returnPressed();

    void on_btnSearchPartner_clicked();

    void on_leUseAccumulated_textChanged(const QString &arg1);

private:
    Ui::WOrder *ui;

    C5User *fUser;

    Working *fWorking;

    WCustomerDisplay *fCustomerDisplay;

    int fGiftCard;

    BHistory fBHistory;

    QJsonObject fAccCard;

    QVector<OGoods> fOGoods;

    bool returnFalse(const QString &msg, C5Database &db);

    bool getDiscountValue(int discountType, double &v);

};

#endif // WORDER_H
