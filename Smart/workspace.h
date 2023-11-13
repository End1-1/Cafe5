#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "c5dialog.h"
#include "dish.h"
#include <QMap>

namespace Ui {
class Workspace;
}

class QTableWidgetItem;
class QTableWidget;
class QListWidgetItem;
class C5User;
class WCustomerDisplay;

class Workspace : public C5Dialog
{
    Q_OBJECT

public:
    explicit Workspace(const QStringList &dbParams);

    ~Workspace();

    bool login();

    virtual void reject() override;

    static Workspace *fWorkspace;

    bool printReceipt(const QString &id, bool printSecond, bool precheck);

    void printService();

    void showCustomerDisplay();

private slots:
    void focusTaxIn();

    void focusLineIn();

    void removeDish(int rownum, const QString &packageuuid);

    void on_tblPart2_itemClicked(QTableWidgetItem *item);

    void on_btnCheckout_clicked();

    void on_btnClearFilter_clicked();

    void on_btnPartUp_clicked();

    void on_btnPartDown_clicked();

    void on_btnDishUp_clicked();

    void on_btnDishDown_clicked();

    void on_btnAny_clicked();

    void on_btnShowPackages_clicked();

    void on_btnShowDishes_clicked();

    void on_lstCombo_itemClicked(QListWidgetItem *item);

    void on_leReadCode_returnPressed();

    void on_btnCostumer_clicked();

    void on_tblDishes_cellClicked(int row, int column);

    void on_btnP1_clicked();

    void on_btnM1_clicked();

    void on_btnP05_clicked();

    void on_btnVoid_clicked();

    void on_btnSetCash_clicked();

    void on_btnSetCard_clicked();

    void on_btnSetIdram_clicked();

    void on_btnSetOther_clicked();

    void on_btnReceived_clicked();

    void on_leReceived_textChanged(const QString &arg1);

    void on_btnAppMenu_clicked();

    void on_btnP05_2_clicked();

    void on_btnP3_clicked();

    void on_btnP4_clicked();

    void on_btnP5_clicked();

    void on_btnP10_clicked();

    void on_btnReprintLastCheck_clicked();

    void on_leReadCode_textChanged(const QString &arg1);

    void on_btnComment_clicked();

    void on_btnMRead_clicked();

    void on_btnHistoryOrder_clicked();

    void on_btnSetCardExternal_clicked();

    void on_tblTables_itemClicked(QTableWidgetItem *item);

    void on_btnSaveAndPrecheck_clicked();

    void on_leCard_textChanged(const QString &arg1);

    void on_leTaxpayerId_textEdited(const QString &arg1);

    void on_btnSetTaxpayer_clicked();

    void on_btnEmarks_clicked();

    void on_printOrder_clicked();

    void on_btnFiscal_clicked(bool checked);

private:
    Ui::Workspace *ui;

    C5User *fUser;

    QList<Dish*> fDishes;

    QHash<QString, Dish*> fDishesBarcode;

    QString fOrderUuid;

    QString fPreviouseUuid;

    QMap<QString, QMap<QString, QVariant> > fAutoDiscounts;

    int fTable;

    QTimer *fTimer;

    int fCustomer;

    int fTypeFilter;

    int fFlagEdited;

    QString fPhone;

    QString fSupplierName;

    int fDiscountId;

    int fDiscountCard;

    int fDiscountMode;

    double fDiscountValue;

    double fDiscountAmount;

    WCustomerDisplay *fCustomerDisplay;

    void configFiscalButton();

    void addDishToOrder(Dish *d);

    double discountValue();

    bool saveOrder(int state);

    int printTax(double cardAmount, double idramAmount);

    void setQty(double qty, int mode, int rownum, const QString &packageuuid);

    void setCustomerPhoneNumber(const QString &number);

    void filter(const QString &name);

    void countTotal();

    void resetOrder();

    void stretchTableColumns(QTableWidget *t);

    void scrollTable(QTableWidget *t, int direction, int rows);

    void updateInfo(int row);
};

#endif // WORKSPACE_H
