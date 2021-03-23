#ifndef DLGORDER_H
#define DLGORDER_H

#include "c5dialog.h"
#include <QTableWidgetItem>
#include <QTimer>

namespace Ui {
class DlgOrder;
}

class C5User;
class C5WaiterOrderDoc;

class DlgOrder : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgOrder();

    ~DlgOrder();

    static void openTable(const QJsonObject &table, C5User *user);

protected:
    virtual void accept();

    virtual void reject();

private:
    Ui::DlgOrder *ui;

    C5User *fUser;

    int fCarNumber;

    C5WaiterOrderDoc *fOrder;

    QString fMenuName;

    QString fPart1Name;

    QTimer fTimer;

    int fTimerCounter;

    void load(int table);

    void buildMenu(const QString &menu, QString part1, QString part2);

    void loadOrder(const QJsonObject &obj);

    void changeQty(double qty);

    void itemsToTable();

    void saveOrder();

    void setServiceLabel();

    void logRecord(const QString &rec, const QString &action, const QString &value1, const QString &value2);

    void processDelayedLogs();

    void setButtonsState();

    void changeTimeOrder();

    void setCar(int num);

    void countHourlyPayment();

private slots:
    void timeout();

    void handleDiscount(const QJsonObject &obj);

    void handleVisit(const QJsonObject &obj);

    void addDishToOrder(const QJsonObject &obj);

    void handleOpenTable(const QJsonObject &obj);

    void handlePrintService(const QJsonObject &obj);

    void saveAndQuit(const QJsonObject &obj);

    void saveAndDiscount(const QJsonObject &obj);

    void changeTable(const QJsonObject &obj);

    virtual void handleError(int err, const QString &msg);

    void on_tblPart1_itemClicked(QTableWidgetItem *item);

    void on_tblPart2_itemClicked(QTableWidgetItem *item);

    void on_tblDishes_itemClicked(QTableWidgetItem *item);

    void on_btnPlus1_clicked();

    void on_btnMin1_clicked();

    void on_btnPlus05_clicked();

    void on_btnMin05_clicked();

    void on_btnCustom_clicked();

    void on_btnPayment_clicked();

    void on_btnExit_clicked();

    void on_btnVoid_clicked();

    void on_btnComment_clicked();

    void on_btnDishScrollDown_clicked();

    void on_btnTypeScrollDown_clicked();

    void on_btnTypeScrollUp_clicked();

    void on_btnDishScrollUp_clicked();

    void on_btnExpandDishTable_clicked();

    void on_btnChangeMenu_clicked();

    void on_btnChangeTable_clicked();

    void on_btnGuest_clicked();

    void on_btnRoomService_clicked();

    void on_btnSearchInMenu_clicked();

    void on_btnCompactDishAddMode_clicked();

    void on_btnTime1_clicked();

    void on_btnTime2_clicked();

    void on_btnTime3_clicked();

    void on_btnCar_clicked();

    void on_btnPackage_clicked();

    void on_btnPrintService_clicked();
};

#endif // DLGORDER_H
