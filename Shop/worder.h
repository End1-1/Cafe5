#ifndef WORDER_H
#define WORDER_H

#include <QDate>
#include <QStyledItemDelegate>
#include <QTime>
#include <QWidget>
#include "struct_partner.h"
#include "struct_waiter_order.h"

namespace Ui
{
class WOrder;
}

class Working;
class C5ClearTableWidget;
class C5User;
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

    void updateCustomerDisplay(WCustomerDisplay *cd);

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

    void setDiscount(const QString &label, const QString &value);

    bool setQtyOfRow(int row, double qty);

    bool setPriceOfRow(int row, double price);

    C5ClearTableWidget* table();

    void checkGoodsCode(const QString &code, std::function<void()> postProcess = nullptr);

    int mSaleTypeMode = 1;

private slots:

    void noImage();

    void on_leCode_textChanged(const QString &arg1);

    void on_leCode_returnPressed();

    void on_btnSearchPartner_clicked();

    void on_leUseAccumulated_textChanged(const QString &arg1);

    void on_btnRemovePartner_clicked();

    void on_btnAddPartner_clicked();

private:
    Ui::WOrder* ui;

    C5User* fUser;

    Working* fWorking;

    WCustomerDisplay* fCustomerDisplay;

    int fGiftCard;

    //BHistory fBHistory;

    WaiterOrder mOrder;

    void parseOrder(const QJsonObject &jdoc);

    void scrollOrderToBottom();

    bool getDiscountValue(int discountType, double &v);

    void setPartner(PartnerItem pi);

    void processCode(const QString &code, int permission, std::function<void (const QString&)> func);

    void checkDiscountCardCode(const QString &code);

    void processPresentCard(const QString &code);

    void processAccumulateCard(const QString &code);

    void printFiscal(std::function<void(const QJsonObject &)> nextStep);

    virtual void showEvent(QShowEvent *e) override;

    void setDiscinfoVisibility(bool v);

signals:
    void orderSaved(QWidget *);
};

#endif // WORDER_H
