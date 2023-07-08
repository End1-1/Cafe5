#ifndef DLGORDER_H
#define DLGORDER_H

#include "c5dialog.h"
#include "c5menu.h"
#include <QTableWidgetItem>
#include <QTimer>

namespace Ui {
class DlgOrder;
}

class C5User;
class C5OrderDriver;
class WOrder;
class C5LineEdit;

class DlgOrder : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgOrder(C5User *user);

    ~DlgOrder();

    C5User *fUser;

    static void openTable(int table, C5User *user);

    static void openTableById(const QString &id, C5User *user);

    void itemsToTable();

    void setStoplistmode();

    bool stoplistMode();

    void viewStoplist();

protected:
    virtual void accept();

    virtual void reject();

private:
    Ui::DlgOrder *ui;

    QDateTime fOpenDateTime;

    int fCarNumber;

    int fTable;

    int fMenuID;

    int fPart1;

    QTimer fTimer;

    int fTimerCounter;

    bool fStoplistMode;

    WOrder *worder();

    QList<WOrder *> worders();

    bool load(int table);

    void disableForCheckall(bool v);

    void buildMenu(int menuid, int part1, int part2);

    void logRecord(const QString &username, const QString &orderid, const QString &rec, const QString &action, const QString &value1, const QString &value2);

    void setButtonsState();

    void removeWOrder(WOrder *wo);

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

    bool worderPaymentOK();

    bool buildDishes(int part2, const QList<DPart2> &dpart2);

    bool fetchDishes(const DPart2 &part2, bool recent, int colcount, int &col, int &row);

    void discountOrder(C5User *u, const QString &code);

private slots:
    void timeout();

    void worderActivated();

    void dishpart1Clicked();

    void processMenuID(int menuid);

    //void handleVisit(const QJsonObject &obj);

    void addDishToOrder(int menuid);

    void handlePrintService(const QJsonObject &obj);

    void handleReceipt(const QJsonObject &obj);

    void handleStopList(const QJsonObject &obj);

    virtual void handleError(int err, const QString &msg);

    void dishPart2Clicked();

    void dishClicked();

    void dishPartClicked();

    void on_btnExit_clicked();

    void on_btnVoid_clicked();

    void on_btnComment_clicked();

    void on_btnChangeMenu_clicked();

    void on_btnSearchInMenu_clicked();

    void on_btnPackage_clicked();

    void on_btnPrintService_clicked();

    void on_btnSit_clicked();

    void on_btnMovement_clicked();

    void on_btnRecent_clicked();

    void on_btnTable_clicked();

    void on_btnChangeStaff_clicked();

    void on_btnScrollUp_clicked();

    void on_btnScrollDown_clicked();

    void on_btnDiscount_clicked();

    void on_btnTotal_clicked();

    void on_btnReceiptLanguage_clicked();

    void on_btnCalcCash_clicked();

    void on_btnCalcCard_clicked();

    void on_btnCalcBank_clicked();

    void on_btnCalcPrepaid_clicked();

    void on_btnCalcOther_clicked();

    void on_btnPaymentCash_clicked();

    void on_btnPaymentCard_clicked();

    void on_btnPaymentBank_clicked();

    void on_btnPrepayment_clicked();

    void on_btnPaymentOther_clicked();

    void on_btnPayCityLedger_clicked();

    void on_btnPayComplimentary_clicked();

    void on_btnPayTransferToRoom_clicked();

    void on_btnReceipt_clicked();

    void on_btnCloseOrder_clicked();

    void on_btnService_clicked();

    void on_btnTax_clicked();

    void on_btnStopListMode_clicked();

    void on_btnOrderComment_clicked();

    void on_btnPlus1_clicked();

    void on_btnMinus1_clicked();

    void on_btnAnyqty_clicked();

    void on_btnDishPart2Down_clicked();

    void on_btnDishPart2Up_clicked();

    void on_btnBillWithoutService_clicked();

    void on_btnFillCash_clicked();

    void on_btnFillCard_clicked();

    void on_btnFillPrepaiment_clicked();

    void on_btnDishDown_clicked();

    void on_btnDishUp_clicked();

    void on_btnPreorder_clicked();

    void on_btnPaymentIdram_clicked();

    void on_btnFillPayX_clicked();

    void on_btnCalcPayX_clicked();

    void on_btnPayX_clicked();

    void on_btnFillIdram_clicked();

    void on_btnCloseCheckAll_clicked();

    void on_btnCheckAll_clicked();

    void on_btnUncheckAll_clicked();

    void on_btnReprintSelected_clicked();

    void on_btnGroupSelect_clicked();

    void on_btnRemoveSelected_clicked();

    void on_btnSetPrecent_clicked();

    void on_btnReceived_clicked();

    void on_btnSelfCost_clicked();

    void on_btnDelivery_clicked();

    void on_btnCalcIdram_clicked();
    void on_btnFillBank_clicked();
    void on_btnCashout_clicked();
};

#endif // DLGORDER_H
