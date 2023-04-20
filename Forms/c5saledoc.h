#ifndef C5SALEDOC_H
#define C5SALEDOC_H

#include "c5widget.h"
#include "cpartners.h"
#include "odraftsale.h"
#include "odraftsalebody.h"
#include <QJsonObject>

namespace Ui {
class C5SaleDoc;
}

static const int PRICEMODE_RETAIL = 1;
static const int PRICEMODE_WHOSALE = 2;

#define REPORT_HANDLER_SALE_DOC_OPEN_DRAFT "39617ca7-8fa4-11ed-8ad3-1078d2d2b808"

class C5SaleDoc : public C5Widget
{
    Q_OBJECT

public:
    explicit C5SaleDoc(const QStringList &dbParams, QWidget *parent = nullptr);
    ~C5SaleDoc();
    void setMode(int mode);
    virtual bool reportHandler(const QString &handleId, const QVariant &data);
    virtual QToolBar *toolBar() override;

private slots:
    void amountDoubleClicked();
    void printSale();
    void createInvoiceAS();
    void createRetailAS();
    void messageResult(QJsonObject jo);
    void saveDataChanges();
    void uuidDoubleClicked();
    void on_PriceTextChanged(const QString &arg1);
    void on_QtyTextChanged(const QString &arg1);
    void on_TotalTextChanged(const QString &arg1);
    void on_leCmd_returnPressed();
    void on_btnAddGoods_clicked();
    void on_btnRemoveGoods_clicked();
    void on_btnEditGoods_clicked();
    void on_btnNewGoods_clicked();
    void on_cbHall_currentIndexChanged(int index);
    void on_btnSearchTaxpayer_clicked();
    void on_btnRemoveDelivery_clicked();
    void on_btnDelivery_clicked();
    void on_btnEditPartner_clicked();

    void on_btnEditAccounts_clicked();

private:
    Ui::C5SaleDoc *ui;
    CPartners fPartner;
    ODraftSale fDraftSale;
    ODraftSaleBody fDraftSaleBody;
    int fMode;
    QAction *fActionSave;
    int addGoods(int goodsId, C5Database &db);
    int addGoods(int store, int goodsId, const QString &barcode, const QString &name, const QString &unitname, double qty, double price);
    void countGrandTotal();
    bool openDraft(const QString &id);
    void setPartner();
    void exportToAs(int doctype);
    bool fOpenedFromDraft;
    QMap<int, QString> fListOfStorages;
};

#endif // C5SALEDOC_H
