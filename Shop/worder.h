#ifndef WORDER_H
#define WORDER_H

#include "c5database.h"
#include "bhistory.h"
#include "oheader.h"
#include "ogoods.h"
#include "odraftsale.h"
#include "ninterface.h"
#include <QWidget>
#include <QDate>
#include <QTime>

namespace Ui
{
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

    C5ClearTableWidget* table();

    bool checkQty(int goods, double qty, QString &err, double oldqty);

    void openDraft(const QString &draftid);

    void checkGoodsCode(const QString &code);

    void addGoods2(const QString &barcode, double price);

private slots:
    void openDraftResponse(const QJsonObject &jdoc);

    void readEmarks();

    void noImage();

    void checkCardClicked(bool v);

    void on_leCode_textChanged(const QString &arg1);

    void on_leCode_returnPressed();

    void on_btnSearchPartner_clicked();

    void on_leUseAccumulated_textChanged(const QString &arg1);

    void on_btnRemovePartner_clicked();

    void on_tblData_doubleClicked(const QModelIndex &index);

private:
    Ui::WOrder* ui;

    C5User* fUser;

    Working* fWorking;

    WCustomerDisplay* fCustomerDisplay;

    int fGiftCard;

    BHistory fBHistory;

    QVector<OGoods> fOGoods;

    NInterface* fHttp;

    bool returnFalse(const QString &msg, C5Database &db);

    bool getDiscountValue(int discountType, double& v);

    void removeDraft();

    void processCode(const QString &code, int permission, std::function<void (const QString&)> func);

    void checkDiscountCardCode(const QString &code);

    void processPresentCard(const QString &code);

    void processAccumulateCard(const QString &code);

};

#endif // WORDER_H
