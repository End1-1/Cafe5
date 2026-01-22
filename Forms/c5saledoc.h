#ifndef C5SALEDOC_H
#define C5SALEDOC_H

#include "c5officewidget.h"
#include "cpartners.h"
#include "odraftsale.h"
#include "odraftsalebody.h"
#include <QJsonObject>

namespace Ui
{
class C5SaleDoc;
}

#define REPORT_HANDLER_SALE_DOC_OPEN_DRAFT "39617ca7-8fa4-11ed-8ad3-1078d2d2b808"

class C5SaleDoc : public C5OfficeWidget
{
    Q_OBJECT

public:
    explicit C5SaleDoc(QWidget *parent = nullptr);

    ~C5SaleDoc();

    void setMode(int mode);

    virtual bool reportHandler(const QString &handleId, const QVariant &data) override;

    virtual QToolBar* toolBar() override;

    bool openDoc(const QString &uuid);

    bool openDraft(const QString &id);

private slots:
    void makeDraftResponse(const QJsonObject &jdoc);

    void removeDocResponse(const QJsonObject &jdoc);

    void amountDoubleClicked();

    void createStoreDocument();

    void printSale();

    void exportXML();

    void fiscale();

    void cancelFiscal();

    void createInvoiceAS();

    void createRetailAS();

    void makeStoreOutput();

    void exportToExcel();

    void returnItems();

    void saveDataChanges();

    void saveAsDraft();

    void saveCopy();

    void removeDoc();

    void uuidDoubleClicked();

    void on_PriceTextChanged(const QString &arg1);

    void on_QtyTextChanged(const QString &arg1);

    void on_discountValueChanged(const QString &arg1);

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

    void on_leCash_textChanged(const QString &arg1);

    void on_btnDeliveryMan_clicked();

    void on_btnQr_clicked();

    void on_btnCalculator_clicked();

    void on_cbStorage_currentIndexChanged(int index);

    void on_btnCashier_clicked();

    void on_btnCopyUUID_clicked();

private:
    Ui::C5SaleDoc* ui;
    CPartners fPartner;
    ODraftSale fDraftSale;
    ODraftSaleBody fDraftSaleBody;
    int fMode;
    QAction* fActionSave;
    QAction* fActionDraft;
    QAction* fActionCopy;
    QAction* fRemoveAction;
    QAction* fPrintTax;
    QAction* fPrintReturnTax;
    QAction* fActionReturn;
    void saveReturnItems();
    int addGoods(int goodsId, C5Database &db);
    int addGoods(const QString &uuid, int store, int goodsId, const QString &barcode, const QString &name,
                 const QString &unitname, double qty, double price, double discount, int isService, const QString &returnFrom, const QString &adgt);
    void countGrandTotal();
    void countTotalQty();
    void setPartner();
    void setPartner(const CPartners &p);
    void setDeliveryMan();
    void exportToAs(int doctype);
    void getFiscalNum();
    bool fOpenedFromDraft;
    QStringList fEmarks;
    QStringList fRowToDelete;
    QMap<int, QString> fListOfStorages;
    QMap<int, double> fSpecialPrices;

};

#endif // C5SALEDOC_H
