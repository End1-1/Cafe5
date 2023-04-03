#ifndef WORDER_H
#define WORDER_H

#include "c5database.h"
#include "c5dbrecord.h"
#include "bhistory.h"
#include "oheader.h"
#include "ogoods.h"
#include "odraftsale.h"
#include <QWidget>
#include <QDate>
#include <QTime>

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

    bool addGoods(int id);

    int addGoodsToTable(int id, bool checkQtyOfStore, const QString &draftid);

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

    void countTotal();

    void setDiscount(const QString &label, const QString &value);

    bool setQtyOfRow(int row, double qty);

    bool setPriceOfRow(int row, double price);

    C5ClearTableWidget *table();

    bool checkQty(int goods, double qty, bool updateStock, QString &err);

private slots:
    void imageLoaded(const QPixmap &img);

    void noImage();

    void on_btnRemove_clicked();

    void on_leCode_textChanged(const QString &arg1);

    void on_leCode_returnPressed();

    void on_leCustomerTaxpayerId_returnPressed();

    void on_btnSearchPartner_clicked();

private:
    Ui::WOrder *ui;

    C5User *fUser;

    Working *fWorking;



    WCustomerDisplay *fCustomerDisplay;

    int fGiftCard;

    BHistory fBHistory;

    QVector<OGoods> fOGoods;

    QByteArray fPreorderUUID;

    bool returnFalse(const QString &msg, C5Database &db);

    bool getDiscountValue(int discountType, double &v);

};

#endif // WORDER_H
