#ifndef DLGORDER_H
#define DLGORDER_H

#include "c5waiterdialog.h"
#include "struct_goods_group.h"
#include "struct_dish.h"
#include "struct_waiter_order.h"
#include "struct_hall.h"
#include "struct_table.h"
#include <QTableWidgetItem>
#include <QTimer>
#include <QStack>

namespace Ui
{
class DlgOrder;
}

class C5User;
class C5OrderDriver;
class WOrder;
class C5LineEdit;
class WaiterOrderItemWidget;
class QDishButton;

class DlgOrder : public C5WaiterDialog
{
    Q_OBJECT

public:
    explicit DlgOrder(C5User *user, HallItem h, TableItem t,  const QVector<GoodsGroupItem*>* groups, const QVector<DishAItem*>* dishes);

    ~DlgOrder();

    void setStoplistmode();

    bool stoplistMode();

    void viewStoplist();

    virtual void accept() override;

    virtual void reject() override;

protected:
    virtual bool eventFilter(QObject *o, QEvent *e) override;

    virtual void showEvent(QShowEvent *e) override;

private:
    Ui::DlgOrder* ui;

    WaiterOrder mOrder;

    QStack<int> mPreviouseParent;

    QToolButton* mBtnGroupsUp;

    QToolButton* mBtnGroupsDown;

    QToolButton* mBtnDishUp;

    QToolButton* mBtnDishDown;

    QToolButton* mBtnOrderUp;

    QToolButton* mBtnOrderDown;

    HallItem mHall;

    TableItem mTable;

    int fMenuID;

    int fPart2Parent;

    bool mShowRemoved = false;

    QTimer fTimer;

    QString mStringBuffer;

    const QVector<GoodsGroupItem*>* mGroups;

    const QVector<DishAItem*>* mDishes;

    int fTimerCounter;

    bool fStoplistMode;

    void makeFavorites();

    void makeGroups(int parent, int dept);

    void makeDishes(int group, int favorite);

    void confirmStringBuffer();

    void disableForCheckall(bool v);

    void createScrollButtons();

    void updateScrollButtonPositions();

    void restoreStoplistQty(int dish, double qty);

    void setLangIcon();

    void calcAmount(C5LineEdit *l);

    void lineEditToHeader();

    void headerToLineEdit();

    void clearOther();

    void setCLComment();

    void setComplimentary();

    void setRoomComment();

    void setSelfcost();

    void setDiscountComment();

    void discountOrder(C5User *u, const QString &code);

    //NEW METHODS
    void createPaymentButtons();

    void configBtnNum();

    int selectedWaiterDishIndex();

    WaiterOrderItemWidget* createOrderItemWidget(WaiterDish d);

    void printPrecheck(const QString &currentStaff);

    void printService(const QJsonObject &jdoc);

    void printRemovedDish(const QJsonObject &jdoc);

    void setDishQty(std::function<double (WaiterDish)> getQty);

    void addDishToOrder(DishAItem *g, QDishButton *btn);

    void funcWithAuth(int permission, const QString &title, std::function<void(C5User*)> function);

private slots:
    void parseOrder(const QJsonObject &jdoc);

    void handleOrderDishClick(const QString &id);

    void setPaymentButtonChecked(bool checked);

    void openReservationResponse(const QJsonObject &jdoc);

    void moveTableResponse(const QJsonObject &jdoc);

    void timeout();

    void qrListResponse(const QJsonObject &obj);

    void handleStopList(const QJsonObject &obj);

    void restoreStoplistQtyResponse(const QJsonObject &jdoc);

    void addStopListResponse(const QJsonObject &jdoc);

    void on_btnExit_clicked();

    void on_btnVoid_clicked();

    void on_btnComment_clicked();

    void on_btnChangeMenu_clicked();

    void on_btnSearchInMenu_clicked();

    void on_btnPackage_clicked();

    void on_btnPrintService_clicked();

    void on_btnSit_clicked();

    void on_btnChangeStaff_clicked();

    void on_btnDiscount_clicked();

    void on_btnTotal_clicked();

    void on_btnReceiptLanguage_clicked();

    void on_btnReceipt_clicked();

    void on_btnCloseOrder_clicked();

    void on_btnService_clicked();

    void on_btnStopListMode_clicked();

    void on_btnOrderComment_clicked();

    void on_btnPlus1_clicked();

    void on_btnMinus1_clicked();

    void on_btnAnyqty_clicked();

    void on_btnCloseCheckAll_clicked();

    void on_btnCheckAll_clicked();

    void on_btnUncheckAll_clicked();

    void on_btnReprintSelected_clicked();

    void on_btnGroupSelect_clicked();

    void on_btnRemoveSelected_clicked();

    void on_btnSetPrecent_clicked();

    void on_btnPartFavorite_clicked();

    void on_btnMenuHome_clicked();

    void on_btnPart1_clicked();

    void on_btnPart2_clicked();

    void on_btnPart3_clicked();

    void on_btnBackGroup_clicked();

    void on_btnShowHideRemoved_clicked(bool checked);

    void on_btnSetWholeAmount_clicked();

    void on_btnNumClear_clicked();

    void on_btnTransferDishes_clicked();

    void on_btnTransferTable_clicked();

signals:

    void orderDishClicked(QString);
};

#endif // DLGORDER_H
