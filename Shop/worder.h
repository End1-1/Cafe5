#ifndef WORDER_H
#define WORDER_H

#include <QDate>
#include <QStyledItemDelegate>
#include <QTime>
#include <QWidget>
#include <functional>
#include "struct_partner.h"
#include "struct_waiter_order.h"
#include "shoployaltystate.h"

namespace Ui
{
class WOrder;
}

class Working;
class C5ClearTableWidget;
class C5User;
class C5LineEdit;
class WCustomerDisplay;

class CustomDelegate : public QStyledItemDelegate
{
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

class WOrder : public QWidget
{
    Q_OBJECT

public:
    explicit WOrder(C5User *user, int saleType, WCustomerDisplay *customerDisplay, QWidget *parent = nullptr);

    ~WOrder();

    void updateCustomerDisplay();

    void clearCode();

    void keyMinus();

    void keyPlus();

    void keyAsterix();

    void writeOrder(std::function<void()> nextStep);

    void fixCostumer(const QString &code);

    void changeQty();

    void changeQty(double qty);

    void changePrice();

    void changePrice(double price);

    int rowCount();

    void removeRow();

    void nextRow();

    void prevRow();

    int lastRow();

    void comma();

    void printPrecheck();

    /** Kitchen/service slip before sale close (marks lines printed on server). */
    void printServiceCheckBeforeSave();

    void printServiceCheck(const QJsonObject &jdoc);

    /** True if any OK line has kitchen printer (f_print1/f_print2). */
    bool needsServicePrint() const;

    void setDiscount(const QString &label, const QString &value);

    bool setQtyOfRow(int row, double qty);

    bool setPriceOfRow(int row, double price);

    C5ClearTableWidget* table();

    void checkGoodsCode(const QString &code, std::function<void()> postProcess = nullptr);

    void checkGoodsId(int goodsId, const QString &scancode = QString(),
                      std::function<void()> postProcess = nullptr, double knownStock = -1);

    int mSaleTypeMode = 1;

    int tableId() const { return mTableId; }

    void setTableId(int tableId);

    int staffId() const { return mStaffId; }

    void setStaffId(int staffId) { mStaffId = staffId; }

    /** Load order from make-draft / reopen without open-table on show. */
    void loadExistingOrder(const QJsonObject &jdoc, int tableId);

private slots:

    void noImage();

    void on_leCode_textChanged(const QString &arg1);

    void on_leCode_returnPressed();

    void on_btnSearchPartner_clicked();

    void on_leUseAccumulated_3_textChanged(const QString &arg1);

    void on_leUseAccumulated_4_textChanged(const QString &arg1);

    void on_btnRemovePartner_clicked();

    void on_btnAddPartner_clicked();

    void on_leComment_editingFinished();

private:
    Ui::WOrder* ui;

    C5User* fUser;

    Working* fWorking;

    ShopLoyaltyState mLoyalty;

    WaiterOrder mOrder;

    int mTableId = 1;
    int mStaffId = 0;
    int mPartnerId = 0;
    bool mSkipOpenTableOnShow = false;

    void parseOrder(const QJsonObject &jdoc);

    void scrollOrderToBottom();

    bool getDiscountValue(int discountType, double &v);

    void setPartner(PartnerItem pi);

    void persistGuestToOrder(std::function<void()> nextStep = nullptr);

    void processCode(const QString &code, int permission, std::function<void (const QString&)> func);

    void checkDiscountCardCode(const QString &code);

    void processPresentCard(const QString &code);

    void processAccumulateCard(const QString &code);

    void printFiscal(std::function<void(const QJsonObject &)> nextStep);

    /** Sum of OK lines for dishId in current order (optionally exclude one line id). */
    double qtyInOrderForDish(int dishId, const QString &excludeLineId = QString()) const;

    /** When dont_allow_negative_remains: refuse if (inOrder + addQty) > stock. */
    bool allowStockForDish(int dishId, bool isService, double stockQty, double addQty,
                           const QString &excludeLineId = QString()) const;

    virtual void showEvent(QShowEvent *e) override;

    void hideLoyaltyPanels();

    void updateLoyaltyLimits();

    void clampUseAmount(C5LineEdit *le);

    QJsonObject loyaltyPayload() const;

signals:
    void orderSaved(QWidget *);
};

#endif // WORDER_H
