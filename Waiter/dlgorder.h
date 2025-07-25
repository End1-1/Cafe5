#ifndef DLGORDER_H
#define DLGORDER_H

#include "c5dialog.h"
#include <QTableWidgetItem>
#include <QTimer>

namespace Ui
{
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

    C5User* fUser;

    static DlgOrder* openTable(int table, C5User *user);

    void setStoplistmode();

    bool stoplistMode();

    void viewStoplist();

protected:
    virtual void accept();

    virtual void reject();

private:
    Ui::DlgOrder* ui;

    QMap<QString, QVariant> fHeader;

    QList<QMap<QString, QVariant> > fBody;

    int fCarNumber;

    int fTable;

    int fMenuID;

    int fPart1;

    int fPart2Parent;

    QTimer fTimer;

    int fTimerCounter;

    bool fStoplistMode;

    QList<WOrder*> worders();

    void updateData();

    void disableForCheckall(bool v);

    void buildMenu(int menuid, int part1, int part2);

    void logRecord(const QString &username, const QString &orderid, const QString &rec, const QString &action,
                   const QString &value1, const QString &value2);

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

    bool buildDishes(int menuid, int part2);

    void discountOrder(C5User *u, const QString &code);

private slots:
    void handleOrderDishClick(const QString &id);

    void openReserveError(const QString &err);

    void openReservationResponse(const QJsonObject &jdoc);

    void openTableResponse(const QJsonObject &jdoc);

    void moveTableResponse(const QJsonObject &jdoc);

    void timeout();

    void worderActivated();

    void dishpart1Clicked();

    void qrListResponse(const QJsonObject &obj);

    //void handleVisit(const QJsonObject &obj);

    void addDishToOrder(int menuid, const QString &emark);

    void handlePrintService(const QJsonObject &obj);

    void handleStopList(const QJsonObject &obj);

    void restoreStoplistQtyResponse(const QJsonObject &jdoc);

    void addStopListResponse(const QJsonObject &jdoc);

    void checkStopListResponse(const QJsonObject &jdoc);

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

    void on_btnMenuSet_clicked();

    void on_btnQR_clicked();

    void on_btnTable_clicked();

    void on_leCmd_returnPressed();

    void on_btnSetQr_clicked();

signals:
    void allDone();

    void orderDishClicked(QString);
};

#endif // DLGORDER_H
