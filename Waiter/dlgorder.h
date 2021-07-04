#ifndef DLGORDER_H
#define DLGORDER_H

#include "c5dialog.h"
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

public slots:
    void changeQty(double qty);

protected:
    virtual void accept();

    virtual void reject();

private:
    Ui::DlgOrder *ui;

    QDateTime fOpenDateTime;

    int fCarNumber;

    int fTable;

    QString fMenuName;

    QString fPart1Name;

    QTimer fTimer;

    int fTimerCounter;

    WOrder *worder();

    QList<WOrder *> worders();

    void load(int table);

    void buildMenu(const QString &menu, QString part1, QString part2);

    void logRecord(const QString &username, const QString &orderid, const QString &rec, const QString &action, const QString &value1, const QString &value2);

    void setButtonsState();

    void removeWOrder(WOrder *wo);

    //void setCar(int num);

    void countHourlyPayment(WOrder *order);

    void restoreStoplistQty(int dish, double qty);

    void setLangIcon();

    void calcAmount(C5LineEdit *l);

    void headerToLineEdit();

    void clearOther();

    void setCLComment();

    void setComplimentary();

    void setRoomComment();

    void setSelfcost();

    void setDiscountComment();

    bool worderPaymentOK();

private slots:
    void timeout();

    void worderActivated();

    void dishpart1Clicked();

    //void handleVisit(const QJsonObject &obj);

    void addDishToOrder(const QJsonObject &obj);

    void handlePrintService(const QJsonObject &obj);

    void handleReceipt(const QJsonObject &obj);

    void handleStopList(const QJsonObject &obj);

    virtual void handleError(int err, const QString &msg);

    void dishPart2Clicked();

    void dishClicked();

    void on_btnExit_clicked();

    void on_btnVoid_clicked();

    void on_btnComment_clicked();

    //void on_btnExpandDishTable_clicked();

    void on_btnChangeMenu_clicked();

    void on_btnGuest_clicked();

    void on_btnSearchInMenu_clicked();

    void on_btnCompactDishAddMode_clicked();

    void on_btnCar_clicked();

    void on_btnPackage_clicked();

    void on_btnPrintService_clicked();

    void on_btnSit_clicked();

    void on_btnSetReserve_clicked();

    void on_btnSplitGuest_clicked();

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
};

#endif // DLGORDER_H
