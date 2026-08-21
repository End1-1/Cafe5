#pragma once

#include <QDate>
#include <QLabel>
#include <QVector>
#include "c5widget.h"
#include "dlgstoreinputpayment.h"
#include "dlgstoreinputpricewarn.h"
#include "office_structs.h"
#include "struct_doc_store_input.h"
#include "storeinputxmlimport.h"

namespace Ui
{
class C5StoreInput;
}

class QTableWidgetItem;
class C5TableWidget;
class C5LineEdit;

class C5StoreInput : public C5Widget
{
    Q_OBJECT

    enum RelatedOutputColumns {
        col_related_id = 0,
        col_related_type,
        col_related_number,
        col_related_status,
        col_related_date,
        col_related_store_out,
        col_related_store_in,
        col_related_sum,
        col_related_doc_type
    };

    enum Columns {
        col_rec_in_id = 0,
        col_goods_id,   // 1
        col_goods_name, // 2
        col_adgt,       // 3
        col_goods_qty,  // 4
        col_goods_unit, // 5
        col_price,      // 6
        col_total,      // 7
        col_valid_date, // 8
        col_comment,    // 9
        col_remain      // 10
    };

public:
    explicit C5StoreInput(C5User *user, const QString &title, QIcon icon, QWidget *parent = nullptr);

    ~C5StoreInput() override;

    void setDocument(StoreInputDocument doc);

    virtual QToolBar* toolBar() override;

    static bool removeDoc(QString id, bool showmessage = true);

    virtual bool allowChangeDatabase() override;

    bool confirmTabClose() override;

    bool confirmApplicationClose() override;

    double total();

    void setStore(int id, const QString &name);

    virtual void hotKey(const QString &key) override;

    bool openDraft(const QString &id, QString &err);

    int addGoods(int goods, const QString &name, double qty, const QString &unit, double price, double total,
                 const QString &comment, const QString &adgt);

    void fillFromInventory(const QList<InventoryDiff> &surpluses);

    bool importInvoiceData(const StoreInputXmlInvoice &invoice,
                           bool askReplaceGoods,
                           const StoreInputXmlImportOptions &options = StoreInputXmlImportOptions());

protected:
    virtual void nextChild() override;

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Ui::C5StoreInput *ui;

    StoreInputDocument mDocData;

    bool fCanChangeFocus;

    QAction *mActionSave = nullptr;

    QAction *mActionDraft = nullptr;

    QString mWebSocketRequestId;

    QDate mInitialDate;
    int mInitialStoreId = 0;
    int mInitialPartnerId = 0;
    int mInitialCurrencyId = 0;
    int mInitialCashboxId = 0;
    int mInitialPaymentTypeId = 0;
    QString mInitialComment;
    QString mInitialDocNum;
    bool mDocumentPersisted = false;
    bool mClosingForNewDocument = false;
    bool mPaidAmountUserEdited = false;
    bool mPriceWarnSuppressed = false;
    bool mSaveBusy = false;
    bool mPriceAskOpen = false;
    int mFocusPriceWarnRow = -1;

    QVector<StorePaymentPreset> mPaymentPresets;
    StorePaymentPreset mCurrentPayment;

    int mRelatedOutputTabIndex = -1;

    bool mRelatedOutputsLoaded = false;

    void captureInitialState();

    void startNewDocument();

    void initPaymentCombo();

    void rebuildPaymentCombo();

    void setPaymentUnpaid();

    void applyPaymentPreset(const StorePaymentPreset &preset);

    int addOrSelectPaymentPreset(const StorePaymentPreset &preset);

    StorePaymentPreset currentPaymentPreset() const;

    double paidAmountFromUi() const;

    void setPaidAmountUi(double amount, bool markUserEdited = false);

    void syncPaidAmountWithPayment();

    void updatePaidAmountEditableState();

    void editPostedPaidAmount();

    void loadRelatedOutputs();

    void openRelatedDocument(const QString &docId, int docType);

    bool hasUnsavedChanges() const;

    double goodsRowPrice(int row) const;

    int priceWarnMode() const;

    int priceWarnPercent() const;

    /** When false (default), rows with price 0 cannot be saved. */
    bool priceAllowZero() const;

    bool priceDeviationExceeded(int row, double price) const;

    bool acceptPriceDeviation(int row, double price) const;

    /** Ask about price jumps before save. Returns false if user rejects any. */
    bool confirmPricesForSave();

    void setPriceWarnSuppressed(bool suppressed) { mPriceWarnSuppressed = suppressed; }

    void syncGoodsSearchCachePrices() const;

    int unsavedCloseChoice() const;

    bool saveDraftBlocking();
    bool savePostedBlocking();
    bool saveBlocking(int status);

    bool buildDoc();

    void setState();

    void correctDishesRows(int row, int count);

    void countTotal();

    bool docCheck(QString &err, int state);

    void rowsCheck(QString &err);

    int addGoodsRow();

    void setDocEnabled(bool v);

    double additionalCost();

    double additionalCostForEveryGoods();

    QString makeGoodsTableHtml(const QStringList &headers, const QList<QStringList>& rows, const QSet<int>& rightCols);

    QString makeOtherChargesHtml(C5TableWidget *tbl, const QStringList &hdr);

    QString makeComplectationInputHtml(const C5LineEdit *code,
                                       const C5LineEdit *name,
                                       const C5LineEdit *qty,
                                       double total,
                                       double qtyVal,
                                       const QStringList &hdr);
private slots:
    void saveDocument();

    void draftDocument();

    void focusNextChildren();

    void changeCurrencyResponse(const QJsonObject &jdoc);

    void slotCheckQtyResponse(const QJsonObject &jdoc);

    void getInput();

    void removeDocument();

    void tblAddChanged(const QString &arg1);

    void tblQtyChanged(const QString &arg1);

    void tblPriceChanged(const QString &arg1);

    void tblTotalChanged(const QString &arg1);

    void on_btnAddGoods_clicked();

    void on_btnNewPartner_clicked();

    void on_btnNewGoods_clicked();

    void on_leScancode_returnPressed();

    void on_btnAddAdd_clicked();

    void on_btnRemoveAdd_clicked();

    void on_btnEditGoods_clicked();

    void on_btnCalculator_clicked();

    void on_btnRememberStoreIn_clicked(bool checked);

    void on_btnCopyUUID_clicked();

    void on_leSearchInDoc_textChanged(const QString &arg1);

    void on_btnCloseSearch_clicked();

    void on_btnChangePartner_clicked();

    void on_btnFixPartner_clicked(bool checked);

    void on_btnCopyLastAdd_clicked();

    void on_btnSaveComment_clicked();

    void on_btnRemoveGoods_clicked();

    void on_btnPinDate_clicked(bool checked);

    void on_tw_currentChanged(int index);

    void on_btnRefreshRelatedOutput_clicked();

    void on_tblRelatedOutput_cellDoubleClicked(int row, int column);

    void importFromXml();
    void printBarcode();
    void on_btnPaymentConfig_clicked();
    void on_cbPayment_currentIndexChanged(int index);
    void onPaidAmountEditingFinished();
    void onPriceWarnSettings();
    void onPriceEditingFinished();

    void lineEditKeyPressed(const QChar &key);
};
